#include "SocketServer.h"


#define SERVER_PORT 12345



SocketServer::SocketServer() {
	std::cout << "in constructor\n" << std::endl;
	initServer();
	printIP();
	
	m_session_manager = new SessionManager();

	initMutex();
	initThreads();
}


/*-------------------------------------------*/
/*                  Basics                   */
/*-------------------------------------------*/
void SocketServer::initServer() {
	setupServerSocketFD();
	
	//m_serverAddress = createIPv4Address("127.0.0.1", SERVER_PORT);
	m_serverAddress = createIPv4Address(SERVER_IP, SERVER_PORT);

	//Use the global namespace bind for the correct bind function (hence the ::bind)
	int bind_result = ::bind(m_serverSocketFD, (struct sockaddr*)m_serverAddress, sizeof(*m_serverAddress));

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
		closesocket(m_serverSocketFD);
		exit(-1);
	}

	// Set socket to be nonblocking. All of the sockets for
	// the incoming connections will also be nonblocking since
	// they will inherit that state from the listening socket.
	u_long on2 = 1;
	rc = ioctlsocket(m_serverSocketFD, FIONBIO, &on2);
	if (rc < 0) {
		perror("ioctl() failed");
		closesocket(m_serverSocketFD);
		exit(-1);
	}
}

void SocketServer::initMutex() {
	//Create compress_fd_array mutex
	m_mutex_compress_flag = CreateMutex(NULL, FALSE, NULL);
	setCompressFDArrayFalse();

	m_mutex_fd_array = CreateMutex(NULL, FALSE, NULL);

	//Create queue control mutex
	m_queue_control_mutex = CreateMutex(NULL, FALSE, NULL);
}

//void SocketServer::check(int input, std::string instance) {
//	if (input == -1) {
//		std::cout << "Error at " << instance << std::endl;
//		exit(2);
//	}
//}

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
	//timeout = (3 * 60 * 1000);

	int		current_fd;
	bool	close_conn;
	do {
		int poll_result = waitForPoll(timeout); //blocks until poll returns


		int current_size = m_nfds;
		for (current_fd = 0; current_fd < current_size; current_fd++) {
			close_conn = false;

			if (m_pollfd_array[current_fd].revents == 0) continue;

			if (m_pollfd_array[current_fd].revents == CLIENT_CLOSED) {
				printf(">>Index %d Connection closed by revents\n", current_fd);
				close_conn = true;
				break;
			} else if (m_pollfd_array[current_fd].revents == CLIENT_CLOSED_PY) {
				printf(">>Index %d Connection closed by revents (py)\n", current_fd);
				close_conn = true;
				break;
			}

			// If revents is not "Read ready", it's an unexpected result
			// log and end the server.
			if ((m_pollfd_array[current_fd].revents & POLLIN) == 0) {
				printf(">>revents is not a \"Read available\" value\n");
				printf(">>revents = %d\n\n", m_pollfd_array[current_fd].revents);
				break;
			}

			// ServerSocketFD is readable. A new connection is available to be made.
			if (m_pollfd_array[current_fd].fd == m_serverSocketFD) {
				printf(">>Server socket is readable. New connection pending...\n");
				pollAcceptNewConnections();
			}

			// Not the listening socket, an existing connection is readable
			else {
				std::string message_string;

				enum SocketReadState state = m_fd_read_state[current_fd];
				switch (state) {
				case awaiting_header:
					//std::cout << "receiving header" << std::endl;
					message_string = pollReceiveMessageHeader(current_fd);
					break;

				case awaiting_body:
					//std::cout << "receiving body" << std::endl;
					message_string = pollReceiveMessageBody(current_fd);
					break;
				}
				//continue case
				if (message_string.compare("continue") == 0) { continue; }

				//Print message
				//else { std::cout << message_string << std::endl; }

				JSON messagejson = JSON::parse(message_string);
				messagejson["source_fd"] = m_pollfd_array[current_fd].fd;

				if (state == awaiting_header) {
					//std::cout << jsontest.at("message_size") << std::endl;
				}
				else {
					//std::cout << jsontest.at("username") << std::endl;
					//std::cout << jsontest.at("message") << std::endl;
					addToQueue(messagejson);
				}
			}  // End of existing connection is readable
		} // End of loop through pollable descriptors

		// If the close_conn flag was turned on, we to clean up this active connection.
		if (close_conn) { closeConnectionFDArray(current_fd); }

		// If the compress_array flag was turned on, we need to squeeze
		// together the array and decrement the number of file descriptors.
		if (m_compress_fd_array) {
			setCompressFDArrayFalse();
			compressFDArray();
		}
	} while (true); // End of server running.

	// Clean up all of the sockets that are open
	closeAllSockets();
}

int SocketServer::waitForPoll(int timeout) {
	int poll_result = WSAPoll(m_pollfd_array, m_nfds, timeout);

	if (poll_result == SOCKET_ERROR) { //Socket error case
		perror("  poll() failed with SOCKET_ERROR. Ending program.\n");
		exit(-1);
	}
	else if (poll_result == 0) { //timeout case
		printf("  poll() timed out. End program.\n");
		exit(-1);
	}

	return poll_result;
}

void SocketServer::pollAcceptNewConnections() {
	do {
		//Accept each incoming connection
		AcceptedSocket* new_connection = acceptIncomingConnection(m_serverSocketFD);
		if (new_connection == NULL) { break; }

		// Add valid incoming connections to the pollfd structure
		SOCKET new_sd = new_connection->m_socketFD;

		std::cout << ">>New incoming connection : " << new_sd << "\n" << std::endl;
		m_pollfd_array[m_nfds].fd = new_sd;
		m_pollfd_array[m_nfds].events = POLLIN;
		m_nfds++;
	} while (true);
}

AcceptedSocket* SocketServer::acceptIncomingConnection(SOCKET serverSocketFD) {
	SOCKADDR_IN clientAddress;
	int clientAddressSize = sizeof(clientAddress);
	SOCKET clientSocketFD = accept(serverSocketFD, (sockaddr*)&clientAddress, &clientAddressSize);

	if (clientSocketFD != INVALID_SOCKET) {
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

std::string SocketServer::pollReceiveMessageHeader(int index) {
	char header_buffer[HEADER_SIZE];
	memset(&header_buffer, 0, sizeof(header_buffer));
	int header_rc = recv(m_pollfd_array[index].fd, header_buffer, sizeof(header_buffer), 0);

	if (header_rc < 0)  return "continue";

	// Header is expected
	if (header_rc != HEADER_SIZE) {
		printf(">>ERROR:Message header was expected but was not received\n");
		return "continue";
	}
	std::string header_string(header_buffer);
	JSON header_json = JSON::parse(header_string);

	int message_size = header_json.at("message_size");
	m_fd_message_size[index] = message_size;
	m_fd_read_state[index] = awaiting_body;

	return header_string;
}

std::string SocketServer::pollReceiveMessageBody(int current_fd) {
	if (m_fd_message_size[current_fd] < 1) {
		printf(">>ERROR:Expected message size is not a valid value\n");
		m_fd_read_state[current_fd] = awaiting_header;
		m_fd_message_size[current_fd] = -1;
		return "continue";
	}

	int message_size = m_fd_message_size[current_fd];
	std::unique_ptr<char[]> message_buffer = std::make_unique<char[]>(message_size);

	//Receive message
	int message_rc = recv(m_pollfd_array[current_fd].fd, message_buffer.get(), message_size, 0);

	if (message_rc < 0) {
		printf(">>ERROR:Nothing was available to read\n");
		m_fd_read_state[current_fd] = awaiting_header;
		m_fd_message_size[current_fd] = -1;
		return "continue";
	}
	std::string message_string(message_buffer.get());
	//std::cout << "Message:" << message_string << "\n" << std::endl;

	m_fd_read_state[current_fd] = awaiting_header;
	m_fd_message_size[current_fd] = -1;  //set to a default value

	return message_string;
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

SOCKET SocketServer::createTCPIPv4Socket() {
	SOCKET a = socket(AF_INET, SOCK_STREAM, 0);
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
	DWORD dwWaitResult = WaitForSingleObject(m_mutex_compress_flag, INFINITE);
	switch (dwWaitResult) {
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		m_compress_fd_array = true;
		// Release ownership of the mutex object
		if (!ReleaseMutex(m_mutex_compress_flag)) {
			printf("Error releasing mutex for m_mutex_compress_flag");
			exit(-1);
		}
		break;

		// The thread got ownership of an abandoned mutex
	case WAIT_ABANDONED:
		return FALSE;
	}
	return true;
}

bool SocketServer::setCompressFDArrayFalse() {
	DWORD dwWaitResult = WaitForSingleObject(m_mutex_compress_flag, INFINITE);
	switch (dwWaitResult) {
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		m_compress_fd_array = false;
		// Release ownership of the mutex object
		if (!ReleaseMutex(m_mutex_compress_flag)) {
			printf("Error releasing mutex for m_mutex_compress_flag");
			exit(-1);
		}
		break;

		// The thread got ownership of an abandoned mutex
	case WAIT_ABANDONED:
		return FALSE;
	}
	return true;
}

bool SocketServer::closeConnectionFDArray(int current_fd) {
	DWORD dwWaitResult = WaitForSingleObject(m_mutex_fd_array, INFINITE);
	switch (dwWaitResult) {
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:
		m_compress_fd_array = false;

		closesocket(m_pollfd_array[current_fd].fd);
		m_pollfd_array[current_fd].fd = -1;
		m_pollfd_array[current_fd].events = 0;
		m_pollfd_array[current_fd].revents = 0;
		setCompressFDArrayTrue();

		// Release ownership of the mutex object
		if (!ReleaseMutex(m_mutex_fd_array)) {
			printf("Error releasing mutex for m_mutex_fd_array");
			exit(-1);
		}
		break;

		// The thread got ownership of an abandoned mutex
	case WAIT_ABANDONED:
		return FALSE;
	}
}

void SocketServer::closeAllSockets() {
	for (int current_fd = 0; current_fd < m_nfds; current_fd++) {
		if (m_pollfd_array[current_fd].fd >= 0)
			closesocket(m_pollfd_array[current_fd].fd);
	}
}

//int SocketServer::sendThroughSocket(SOCKET destination, std::string message_str) {
//	const char* message_char = message_str.c_str();
//	int send_result = send(destination, message_char, (int)strlen(message_char), 0);
//
//	if (send_result == SOCKET_ERROR) {
//		std::cout << "Error sending message to socket " << destination << "." << std::endl;
//		return -1;
//	}
//
//	return 0;
//}

void SocketServer::testSendToAllOthers(SOCKET source, std::string message) {
	for (int i = 0; i < m_nfds; i++) {
		SOCKET dest = m_pollfd_array[i].fd;
		if (dest != source && dest != m_serverSocketFD) {
			int send_result = common::sendThroughSocket(dest, message);
		}
	}
}


/*-------------------------------------------*/
/*        Multithreading / ThreadPool        */
/*-------------------------------------------*/
int SocketServer::initThreads() {
	//Create worker threads
	for (int i = 0; i < MAX_THREADS; i++) {
		ThreadRoleArray[i].server = this;

		if (i == 0)
			ThreadRoleArray[i].role = listen_incoming_data;
		else
			ThreadRoleArray[i].role = work;

		hThreadArray[i] = CreateThread(
			NULL,
			0,
			staticThreadFunction,
			&(ThreadRoleArray[i]),
			0,
			&dwThreadIDArray[i]);

		if (hThreadArray[i] == NULL) {
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}
	std::cout << "threads created" << std::endl;
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
	std::cout << "wait passed" << std::endl;

	// Close thread and mutex handles
	for (int i = 0; i < MAX_THREADS; i++)
		CloseHandle(hThreadArray[i]);

	CloseHandle(m_queue_control_mutex);

	return 0;
}

//Windows CreateThread is weird and needs this static thread function wrapper
DWORD WINAPI SocketServer::staticThreadFunction(void* param) {
	ThreadRoleStruct* role_info = (ThreadRoleStruct*)param;

	SocketServer* this_server = role_info->server;
	return this_server->threadFunction(&(role_info->role));
}

DWORD WINAPI SocketServer::threadFunction(ThreadRoleEnum* role) {
	while (true) {
		switch (*role) {
			case listen_incoming_data:
				pollSocketArray();
				break;

			case work:
				JSON removed = getWorkFromQueue();

				if (removed != NULL) {
					/*std::cout << "username:" << removed.at("username") << std::endl;
					std::cout << "message:" << removed.at("message") << std::endl;
					std::cout << "message thread ID" << GetCurrentThreadId() << "\n" << std::endl;

					std::string username(removed.at("username"));
					std::string message_contents(removed.at("message"));
					std::string testmessage = "\n" + username + ":" + message_contents;
					testSendToAllOthers(removed.at("source_fd"), testmessage);*/


				}
				break;
		}
	}
	return NULL;
}

int SocketServer::addToQueue(JSON task) {
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(
		m_queue_control_mutex,    // handle to mutex
		INFINITE);  // no time-out interval

	switch (dwWaitResult) {
		// The thread has ownership of the mutex
		case WAIT_OBJECT_0:
			m_work_queue.push(task);

			// Release ownership of the mutex object
			if (!ReleaseMutex(m_queue_control_mutex)) {
				std::cout << "Mutex was not released properly." << std::endl;
			}
			break;

		// The thread got ownership of an abandoned mutex
		case WAIT_ABANDONED:
			return -1;
	}

	return 0;
}

JSON SocketServer::getWorkFromQueue() {
	DWORD dwWaitResult;
	JSON ret = NULL;

	dwWaitResult = WaitForSingleObject(
		m_queue_control_mutex,    // handle to mutex
		INFINITE);  // no time-out interval

	switch (dwWaitResult) {
		// The thread has ownership of the mutex
		case WAIT_OBJECT_0:
			if (m_work_queue.size() != 0) {
				ret = m_work_queue.front();
				m_work_queue.pop();
			}

			// Release ownership of the mutex object
			if (!ReleaseMutex(m_queue_control_mutex))
				std::cout << "Mutex was not released properly at getWorkQueueLength." << std::endl;

			break;

			// The thread got ownership of an abandoned mutex
		case WAIT_ABANDONED:
			return FALSE;
		}

	return ret;
}







