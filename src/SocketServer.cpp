#include "SocketServer.h"


/*-------------------------------------------*/
/*                Constructor                */
/*-------------------------------------------*/
SocketServer::SocketServer() {
	std::cout << "Constructing SocketServer..." << std::endl;
	initServer();
	// printIP();

	m_session_manager = new SessionManager();
	std::fill_n(m_fd_message_size, FD_ARRAY_SIZE, HEADER_SIZE); //initialize all sockets to expect headers

	initMutex();
}



/*-------------------------------------------*/
/*                  Basics                   */
/*-------------------------------------------*/

void SocketServer::runServerAsAppServer() {
	initThreadsAppServer();
}


void SocketServer::runServerStandalone() {
	initThreadsStandalone();
}

void SocketServer::initServer() {
	setupServerSocketFD();

	m_serverAddress = createIPv4Address(SERVER_IP, SERVER_PORT);

	//Use the global namespace bind for the correct bind function (hence the ::bind)
	int bind_result = bind(m_serverSocketFD, (struct sockaddr*)m_serverAddress, sizeof(*m_serverAddress));

	//std::cout << bind_result << "\n" << std::endl;
	common::check(bind_result, "Server bind");
	std::cout << "Server socket bound successfullly\n" << std::endl;
}

void SocketServer::setupServerSocketFD() {
	m_serverSocketFD = createTCPIPv4Socket();

	// Allow socket descriptor to be reusable
	int on = 1;
	int rc = setsockopt(
		m_serverSocketFD,
		SOL_SOCKET,
		SO_REUSEADDR,
		(const char*)&on,
		sizeof(on)
	);

	if (rc < 0) {
		perror("setsockopt() failed");
		close(m_serverSocketFD);
		exit(-1);
	}

	// Set socket to be nonblocking. All of the sockets for
	// the incoming connections will also be nonblocking since
	// they will inherit that state from the listening socket.
	u_long on2 = 1;
	rc = ioctl(m_serverSocketFD, FIONBIO, &on2);
	if (rc < 0) {
		perror("ioctl() failed");
		close(m_serverSocketFD);
		exit(-1);
	}
}

void SocketServer::initMutex() {
	//Create compress_fd_array mutex
	m_mutex_compress_flag.unlock();
	setCompressFDArrayFalse();

	m_mutex_fd_array.unlock();

	//Create queue control mutex
	m_queue_control_mutex.unlock();
}

void SocketServer::printIP() {
	char buffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &m_serverAddress->sin_addr, buffer, sizeof(buffer));
	std::cout << buffer << ":" << m_serverAddress->sin_port << std::endl;
}


/*-------------------------------------------*/
/*        Poll and Socket Connections        */
/*-------------------------------------------*/
void SocketServer::pollSocketArray() {
	// Set the listen back log
	int		listen_result = listen(m_serverSocketFD, LISTEN_BACKLOG);
	common::check(listen_result, "poll listen");

	// Set index 0 to be the serverSocketFD
	memset(m_pollfd_array, 0, sizeof(m_pollfd_array));
	m_pollfd_array[0].fd = m_serverSocketFD;
	m_pollfd_array[0].events = POLLIN;
	m_nfds = 1;

	int		timeout = -1;
	// timeout = (5 * 60 * 1000);  // 5 minute timeout

	int		fd_index, current_fd;
	bool	close_conn;
	do {
		int poll_result = waitForPoll(timeout); //blocks until poll returns
		int current_size = m_nfds;

		for (fd_index = 0; fd_index < current_size; fd_index++) {
			current_fd = m_pollfd_array[fd_index].fd;
			close_conn = false;

			// No updates on this socket
			if (m_pollfd_array[fd_index].revents == 0) continue;

			// If revents is not "Read ready", it's an unexpected result
			// log and end the server.
			if ((m_pollfd_array[fd_index].revents & POLLIN) == 0) {
				printf(">>revents is not a \"Read available\" value\n");
				printf(">>revents = %d\n\n", m_pollfd_array[fd_index].revents);
				break;
			}

			// ServerSocketFD is readable. A new connection is available to be made.
			if (m_pollfd_array[fd_index].fd == m_serverSocketFD) {
				printf(">>Server socket is readable. New connection pending...\n");
				pollAcceptNewConnections();
			}

			// Not the listening socket, an existing connection is readable
			else {
				SocketReadState socket_state = m_fd_read_state[fd_index];
				if (socket_state == awaiting_WebSocket_upgrade) {
					common::print("upgrading websocket (should not be here)");
					doWebsocketUpgrade(fd_index);
				}

				else if (socket_state == debug_print) {
					common::print("debug printing (should not be here)");
					char message_buffer[1024];
					int message_rc = recv(current_fd, message_buffer, 1024, 0);

					std::string message_str(message_buffer);

					std::cout << message_str << std::endl;
				}

				else {
					pollReceiveAndProcessMessage(fd_index);
				}
			}

		} // End of loop through pollable descriptors

		// If the compress_array flag was turned on, we need to squeeze
		// together the array and decrement the number of file descriptors.
		if (m_compress_fd_array) {
			compressFDArray();
			setCompressFDArrayFalse();
		}
	} while (true); // End of server running.

	// Clean up all of the sockets that are open
	closeAllSockets();
}

int SocketServer::waitForPoll(int timeout) {
	int poll_result = poll(m_pollfd_array, m_nfds, timeout);

	if (poll_result == -1) { //Socket error case
		perror("  poll() failed with SOCKET_ERROR. Ending program.\n");
		exit(-1);
	}

	if (poll_result == 0) {
		std::cout << "Poll has timed out. Closing server." << std::endl;
		exit(-10);
	}

	return poll_result;
}

void SocketServer::pollAcceptNewConnections() {
	do {
		//Accept each incoming connection
		AcceptedSocket* new_connection = acceptIncomingConnection(m_serverSocketFD);
		if (new_connection == NULL) { break; }

		// Add valid incoming connections to the pollfd structure
		int new_sd = new_connection->m_socketFD;

		std::cout << ">>New incoming connection : " << new_sd << "\n" << std::endl;
		m_pollfd_array[m_nfds].fd = new_sd;
		m_pollfd_array[m_nfds].events = POLLIN;

		m_fd_read_state[m_nfds] = awaiting_header;

		// int message_size = 1024;
		// std::unique_ptr<char[]> message_buffer = std::make_unique<char[]>(message_size);
		// int message_rc = recv(new_sd, message_buffer.get(), 1024, 0);


		m_nfds++;
	} while (true);
}

AcceptedSocket* SocketServer::acceptIncomingConnection(int serverSocketFD) {
	sockaddr_in clientAddress;
	int clientAddressSize = sizeof(clientAddress);
	int clientSocketFD = accept(serverSocketFD, (sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);

	if (clientSocketFD != -1) {
		AcceptedSocket* acceptedSocket = new AcceptedSocket();
		acceptedSocket->m_address = clientAddress;
		acceptedSocket->m_socketFD = clientSocketFD;
		acceptedSocket->m_accepted_successful = clientSocketFD > 0;

		if (!acceptedSocket->m_accepted_successful) {
			acceptedSocket->m_error = clientSocketFD;
		}

		//std::cout << "New connection:" << acceptedSocket->toString() << std::endl;
		return acceptedSocket;
	}

	return NULL;
}

void SocketServer::pollReceiveAndProcessMessage(int fd_index) {
	int current_fd = m_pollfd_array[fd_index].fd;
	enum SocketReadState state = m_fd_read_state[fd_index];

	int message_size = m_fd_message_size[fd_index];
	std::unique_ptr<char[]> message_buffer = std::make_unique<char[]>(message_size);


	//Receive message
	int message_rc = recv(current_fd, message_buffer.get(), message_size, 0);
	if (message_rc == 0) {
		std::cout << "Client on index " << fd_index << " has closed their connection." << std::endl;
		closeConnectionFDArray(fd_index);
		m_session_manager->removeSocket(current_fd);
		return;
	}

	std::string message_string(message_buffer.get());
	JSON message_json = JSON::parse(message_string);

	if (state == awaiting_header) {  // Prepare for incoming body
		int next_message_size = message_json.at("message_size");
		m_fd_message_size[fd_index] = next_message_size;
		m_fd_read_state[fd_index] = awaiting_body;
	}
	else if (state == awaiting_body) { // Prepare for next header
		m_fd_read_state[fd_index] = awaiting_header;
		m_fd_message_size[fd_index] = HEADER_SIZE;  //set to a default value

		message_json["source_fd"] = m_pollfd_array[fd_index].fd;
		addToQueue(message_json);
	}

	// PRINTING
	// std::cout << message_string << std::endl;
	// if (state == awaiting_header) {
	// 	std::cout << jsontest.at("message_size") << std::endl;
	// } else {
	// 	std::cout << jsontest.at("username") << std::endl;
	// 	std::cout << jsontest.at("message") << std::endl;
	// }
}

struct sockaddr_in* SocketServer::createIPv4Address(std::string ip, int port) {
	struct sockaddr_in* address = new sockaddr_in;
	address->sin_family = AF_INET;
	address->sin_port = htons(port);

	if (ip.length() == 0) {
		address->sin_addr.s_addr = INADDR_ANY;
	}
	else {
		inet_pton(AF_INET, ip.c_str(), &(address->sin_addr.s_addr));
	}

	return address;
}

int SocketServer::createTCPIPv4Socket() {
	int a = socket(AF_INET, SOCK_STREAM, 0);
	return a;
}

void SocketServer::compressFDArray() {
	for (int current_fd = 0; current_fd < m_nfds; current_fd++) {
		if (m_pollfd_array[current_fd].fd == -1) {
			for (int compress_index = current_fd; compress_index < m_nfds - 1; compress_index++) {
				m_pollfd_array[compress_index] = m_pollfd_array[compress_index + 1];
				m_fd_read_state[compress_index] = m_fd_read_state[compress_index + 1];
				m_fd_message_size[compress_index] = m_fd_message_size[compress_index + 1];
			}
			current_fd--;
			m_nfds--;
		}
	}
}

bool SocketServer::setCompressFDArrayTrue() {
	m_mutex_compress_flag.lock();

	m_compress_fd_array = true;

	m_mutex_compress_flag.unlock();
	return true;
}

bool SocketServer::setCompressFDArrayFalse() {
	m_mutex_compress_flag.lock();

	m_compress_fd_array = false;

	m_mutex_compress_flag.unlock();
	return true;
}

bool SocketServer::closeConnectionFDArray(int current_fd) {
	m_mutex_fd_array.lock();
	setCompressFDArrayFalse();

	close(m_pollfd_array[current_fd].fd);
	m_pollfd_array[current_fd].fd = -1;
	m_pollfd_array[current_fd].events = 0;
	m_pollfd_array[current_fd].revents = 0;
	setCompressFDArrayTrue();

	m_mutex_fd_array.unlock();

	return true;
}

void SocketServer::closeAllSockets() {
	for (int current_fd = 0; current_fd < m_nfds; current_fd++) {
		if (m_pollfd_array[current_fd].fd >= 0)
			close(m_pollfd_array[current_fd].fd);
	}
}

void SocketServer::testSendToAllOthers(int source, std::string message) {
	for (int i = 0; i < m_nfds; i++) {
		int dest = m_pollfd_array[i].fd;
		if (dest != source && dest != m_serverSocketFD) {
			int send_result = common::sendThroughSocket(dest, message);
		}
	}
}

std::string SocketServer::computeSecWebsocketAccept(std::string SecWebsocketKey) {
	std::string key_appended = SecWebsocketKey + WEBSOCKET_MAGIC;
	const unsigned char* to_encode = (const unsigned char*)(key_appended.c_str());
	unsigned char hash[20];

	SHA1(to_encode, sizeof(to_encode) - 1, hash);
	std::string accept_encoded = common::base64_encode(hash, 20);

	return accept_encoded;
}

void SocketServer::doWebsocketUpgrade(int fd_index) {
	int current_fd = m_pollfd_array[fd_index].fd;

	int message_size = 1024;
	std::unique_ptr<char[]> message_buffer = std::make_unique<char[]>(message_size);

	int message_rc = recv(current_fd, message_buffer.get(), 1024, 0);

	std::string message_string(message_buffer.get());
	std::cout << message_string << std::endl;
	std::cout << message_string.length() << std::endl;

	std::string upgrade_request(message_buffer.get());

	std::string websocket_key = extractWebsocketKey(upgrade_request);

	std::cout << websocket_key << std::endl;

	std::string upgrade_response = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
	upgrade_response += computeSecWebsocketAccept(websocket_key);
	upgrade_response += "\r\n\r\n";

	common::sendThroughSocket(current_fd, upgrade_response);
	m_fd_read_state[fd_index] = awaiting_header;
	// m_fd_read_state[fd_index] = debug_print;
}

std::string SocketServer::extractWebsocketKey(std::string http_request) {
	std::string s = http_request;
	int length = s.length();

	std::string token = "Sec-WebSocket-Key: ";
	int start = s.find(token) + token.length();

	std::string key_to_end = s.substr(start, s.length() - 1);

	int end = key_to_end.find("\r\n");

	std::string key = s.substr(start, end);

	return key;
}


/*-------------------------------------------*/
/*        Multithreading / ThreadPool        */
/*-------------------------------------------*/
int SocketServer::initThreadsStandalone() {
	//Create worker threads

	int thread_start = THREAD_START;
	for (int i = thread_start; i < MAX_THREADS; i++) {
		m_thread_role_array[i].server = this;
		if (i == 0)
			m_thread_role_array[i].role = listen_incoming_data;
		else
			m_thread_role_array[i].role = work;

		pthread_create(&m_thread_pool[i], NULL, staticThreadFunction, &(m_thread_role_array[i]));
	}
	std::cout << "threads created" << std::endl;

	for (int i = thread_start; i < MAX_THREADS; i++)
		pthread_join(m_thread_pool[i], NULL);

	std::cout << "wait passed" << std::endl;

	// Close thread and mutex handles?? unsure if needed for linux

	return 0;
}

void* SocketServer::staticThreadFunction(void* args) {
	ThreadRoleStruct* thread_role = (ThreadRoleStruct*)args;
	return thread_role->server->threadFunction(thread_role->role);
}


void* SocketServer::threadFunction(ThreadRoleEnum role) {
	JSON removed_json;

	while (true) {
		switch (role) {
		case listen_incoming_data:
			pollSocketArray();
			break;

		case work:
			// JSON removed = getWorkFromQueue();

			removed_json = getWorkFromQueue();

			if (!common::stringCompare(removed_json.at("stage"), "empty")) {

				// Printing
				if (common::stringCompare(removed_json.at("stage"), "chat")) {
					std::cout << "username:" << removed_json.at("username") << std::endl;
					std::cout << "message:" << removed_json.at("message") << std::endl;
					std::cout << "message thread ID" << syscall(__NR_gettid) << "\n" << std::endl;

					std::string username(removed_json.at("username"));
					std::string message_contents(removed_json.at("message"));
					std::string testmessage = "\n" + username + ":" + message_contents;
				}

				m_session_manager->receiveJSON(removed_json);
			}
			break;
		}
	}
	return NULL;
}


void* SocketServer::threadFunction_ws(ThreadRoleEnum role) {

	std::pair<JSON, ws_conn_hdl> removed_pair;
	JSON removed_json;
	ws_conn_hdl removed_hdl;

	while (true) {
		switch (role) {
		case listen_incoming_data:
			pollSocketArray();
			break;

		case work:
			// JSON removed = getWorkFromQueue();

			removed_pair = getWorkFromQueue_ws();
			removed_json = removed_pair.first;
			removed_hdl = removed_pair.second;

			// Printing
			if (!common::stringCompare(removed_json.at("stage"), "empty")) {
				if (common::stringCompare(removed_json.at("stage"), "chat")) {
					std::cout << "username:" << removed_json.at("username") << std::endl;
					std::cout << "message:" << removed_json.at("message") << std::endl;
					std::cout << "message thread ID" << syscall(__NR_gettid) << "\n" << std::endl;

					std::string username(removed_json.at("username"));
					std::string message_contents(removed_json.at("message"));
					std::string testmessage = "\n" + username + ":" + message_contents;
				}
				//testSendToAllOthers(removed.at("source_fd"), testmessage);
				// m_session_manager->receiveJSON(removed);

				m_session_manager->receiveWork_ws(removed_pair);
			}
			break;
		}
	}
	return NULL;
}


int SocketServer::addToQueue(JSON task) {
	m_queue_control_mutex.lock();
	m_work_queue.push(task);
	m_queue_control_mutex.unlock();
	return 0;
}


int SocketServer::addToQueue_ws(std::string task, ws_conn_hdl hdl) {
	JSON task_json = JSON::parse(task);
	return addToQueue_ws(task_json, hdl);
}

int SocketServer::addToQueue_ws(JSON task, ws_conn_hdl hdl) {
	m_queue_control_mutex.lock();
	m_work_queue.push(task);
	m_hdl_queue.push(hdl);
	m_queue_control_mutex.unlock();
	return 0;
}

JSON SocketServer::getWorkFromQueue() {
	m_queue_control_mutex.lock();
	JSON ret = JSON::parse("{\"stage\" : \"empty\"}");
	if (m_work_queue.size() != 0) {
		ret = m_work_queue.front();
		m_work_queue.pop();
	}
	m_queue_control_mutex.unlock();

	return ret;
}

std::pair<JSON, ws_conn_hdl> SocketServer::getWorkFromQueue_ws() {
	m_queue_control_mutex.lock();

	std::pair<JSON, ws_conn_hdl> ret;

	ret.first = JSON::parse("{\"stage\" : \"empty\"}");

	if (m_work_queue.size() != 0) {
		ret.first = m_work_queue.front();
		m_work_queue.pop();

		ret.second = m_hdl_queue.front();
		m_hdl_queue.pop();
	}


	m_queue_control_mutex.unlock();

	return ret;
}


/////APP SERVER


void SocketServer::initThreadsAppServer() {
	//Create worker threads

	int thread_start = THREAD_START;
	for (int i = thread_start; i < MAX_THREADS; i++) {
		m_thread_role_array[i].server = this;
		if (i == 0)
			m_thread_role_array[i].role = listen_incoming_data;
		else
			m_thread_role_array[i].role = work;

		pthread_create(&m_thread_pool[i], NULL, staticThreadFunction, &(m_thread_role_array[i]));
	}
	std::cout << "threads created" << std::endl;

	for (int i = thread_start; i < MAX_THREADS; i++)
		pthread_join(m_thread_pool[i], NULL);

	std::cout << "wait passed" << std::endl;
}

void* SocketServer::staticThreadFunctionAppServer(void* args) {
	ThreadRoleStruct* thread_role = (ThreadRoleStruct*)args;
	return thread_role->server->threadFunctionAppServer(thread_role->role);
}


void* SocketServer::threadFunctionAppServer(ThreadRoleEnum role) {
	JSON removed_json;

	while (true) {
		switch (role) {
		case listen_incoming_data:
			pollSocketArrayAppServer();
			break;

		case work:
			removed_json = getWorkFromQueue();

			if (!common::stringCompare(removed_json.at("stage"), "empty")) {
				// Printing
				if (common::stringCompare(removed_json.at("stage"), "chat")) {
					std::cout << "username:" << removed_json.at("username") << std::endl;
					std::cout << "message:" << removed_json.at("message") << std::endl;
					std::cout << "message thread ID" << syscall(__NR_gettid) << "\n" << std::endl;

					std::string username(removed_json.at("username"));
					std::string message_contents(removed_json.at("message"));
					std::string testmessage = "\n" + username + ":" + message_contents;
				}

				m_session_manager->receiveJSON(removed_json);
			}
			break;
		}
	}
	return NULL;
}

void SocketServer::pollSocketArrayAppServer() {
	// Set the listen back log
	int		listen_result = listen(m_serverSocketFD, LISTEN_BACKLOG);
	common::check(listen_result, "poll listen");

	// Set index 0 to be the serverSocketFD
	memset(m_pollfd_array, 0, sizeof(m_pollfd_array));
	m_pollfd_array[0].fd = m_serverSocketFD;
	m_pollfd_array[0].events = POLLIN;
	m_nfds = 1;

	int		timeout = -1;
	// timeout = (5 * 60 * 1000);  // 5 minute timeout

	int		fd_index, current_fd;
	bool	close_conn;
	do {
		int poll_result = waitForPoll(timeout); //blocks until poll returns
		int current_size = m_nfds;

		//ONLY DIFFERENCE IN THIS APPSERVER VERSION
		// Only the socketio server should be pollable
		current_size = current_size > 2 ? 2 : current_size;

		for (fd_index = 0; fd_index < current_size; fd_index++) {
			current_fd = m_pollfd_array[fd_index].fd;
			close_conn = false;

			// No updates on this socket
			if (m_pollfd_array[fd_index].revents == 0) continue;

			// If revents is not "Read ready", it's an unexpected result
			// log and end the server.
			if ((m_pollfd_array[fd_index].revents & POLLIN) == 0) {
				printf(">>revents is not a \"Read available\" value\n");
				printf(">>revents = %d\n\n", m_pollfd_array[fd_index].revents);
				break;
			}

			// ServerSocketFD is readable. A new connection is available to be made.
			if (m_pollfd_array[fd_index].fd == m_serverSocketFD) {
				printf(">>Server socket is readable. New connection pending...\n");
				pollAcceptNewConnections();
			}

			// Not the listening socket, an existing connection is readable
			else {
				SocketReadState socket_state = m_fd_read_state[fd_index];
				if (socket_state == awaiting_WebSocket_upgrade) {
					common::print("upgrading websocket (should not be here)");
					doWebsocketUpgrade(fd_index);
				}

				else if (socket_state == debug_print) {
					common::print("debug printing (should not be here)");
					char message_buffer[1024];
					int message_rc = recv(current_fd, message_buffer, 1024, 0);

					std::string message_str(message_buffer);

					std::cout << message_str << std::endl;
				}

				else {
					pollReceiveAndProcessMessage(fd_index);
				}
			}

		} // End of loop through pollable descriptors

		// If the compress_array flag was turned on, we need to squeeze
		// together the array and decrement the number of file descriptors.
		if (m_compress_fd_array) {
			compressFDArray();
			setCompressFDArrayFalse();
		}
	} while (true); // End of server running.

	// Clean up all of the sockets that are open
	closeAllSockets();
}