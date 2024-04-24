#include "SocketServer.h"


#define SERVER_PORT 12345


SocketServer::SocketServer() {
	std::cout << "in constructor\n" << std::endl;
	initServer();
	printIP();

	pollTest();

	//initFDSet();
	//initThreads();
	
	/*while (true) {
		int listen_result = listen(m_serverSocketFD, 10);
		AcceptedSocket* new_connection = new AcceptedSocket;
		new_connection = acceptIncomingConnection(m_serverSocketFD);
		m_client_list.insert(new_connection);
	}*/
}


void SocketServer::initServer() {
	setupServerSocketFD();

	
	//m_serverAddress = createIPv4Address("", SERVER_PORT);
	//m_serverAddress = createIPv4Address("127.0.0.1", SERVER_PORT);
	m_serverAddress = createIPv4Address("192.168.0.77", SERVER_PORT);

	int bind_result = bind(m_serverSocketFD, (struct sockaddr*)m_serverAddress, sizeof(*m_serverAddress));
	std::cout << bind_result << "\n" << std::endl;
	check(bind_result, "Server bind");
	std::cout << "Server socket bound successfullly" << std::endl;
}


void SocketServer::pollTest() {
	int    len, rc, on = 1;
	int    new_sd = -1;
	int    desc_ready, end_server = FALSE, compress_array = FALSE;
	int    close_conn;
	char   header_buffer[HEADER_SIZE];
	int    current_size = 0, current_fd, compress_index;
	nfds = 1;

	AcceptedSocket* new_connection;

	// Set the listen back log
	rc = listen(m_serverSocketFD, 32);
	if (rc < 0) {
		perror("listen() failed");
		closesocket(m_serverSocketFD);
		exit(-1);
	}

	memset(fds, 0, sizeof(fds));

	fds[0].fd = m_serverSocketFD;
	fds[0].events = POLLIN;

	int timeout = -1;
	//timeout = (3 * 60 * 1000);

	int poll_result;
	do {
		//printf("Waiting on poll()...\n");
		poll_result = WSAPoll(fds, nfds, timeout);

		if (poll_result == SOCKET_ERROR) {
			//Socket error case
			perror("  poll() failed with SOCKET_ERROR");
			break;
		}
		else if (poll_result == 0) {
			//timeout case
			printf("  poll() timed out.  End program.\n");
			break;
		}

		// One or more descriptors are readable. Need to
		// determine which ones they are.
		current_size = nfds;
		for (current_fd = 0; current_fd < current_size; current_fd++) {
			close_conn = FALSE;

			if (fds[current_fd].revents == 0)
				continue;

			if (fds[current_fd].revents == CLIENT_CLOSED) {
				printf(">>Index %d Connection closed by revents\n", current_fd);
				close_conn = TRUE;
				break;
			}

			// If revents is not "Read ready", it's an unexpected result
			// log and end the server.
			if ((fds[current_fd].revents & POLLIN) == 0) {
				printf(">>revents is not a \"Read available\" value\n");
				printf(">>revents = %d\n\n", fds[current_fd].revents);
				end_server = TRUE;
				break;
			}

			// ServerSocketFD is readable. A new connection is available to be made.
			if (fds[current_fd].fd == m_serverSocketFD) {
				printf(">>Server socket is readable. New connection pending...\n");

				do {
					//Accept each incoming connection
					new_connection = acceptIncomingConnection(m_serverSocketFD);

					// Add valid incoming connections to the pollfd structure
					if (new_connection != NULL) {
						new_sd = new_connection->m_socketFD;
						printf(">>New incoming connection : %d\n\n", new_sd);
						fds[nfds].fd = new_sd;
						fds[nfds].events = POLLIN;
						nfds++;
					}
				} while (new_connection != NULL);
			}

			// This is not the listening socket, therefore an
			// existing connection must be readable
			else {
				do {
					if (fd_read_state[current_fd] == awaiting_header) {
						//Recieve Header Case
						printf("Receiving Header:\n");

						memset(&header_buffer, 0, sizeof(header_buffer));
						int header_rc = recv(fds[current_fd].fd, header_buffer, sizeof(header_buffer), 0);

						if (header_rc < 0)  //No more data to read
							break;

						if (header_rc == 0) {
							printf("  Connection closed in \"reading existing connection\"\n");
							close_conn = TRUE;
							break;
						}

						//printf("header_rc:%d\n", header_rc);

						// Header is expected
						if (header_rc != HEADER_SIZE) {
							printf(">>ERROR:Message header was expected but was not received\n");
							break;
						}
						/*else
							header_buffer[header_rc - 1] = 0;*/

						len = header_rc;

						printf(">>ClientFD:%d\n", (int)fds[current_fd].fd);
						printf("Header:%s\n", header_buffer);
						//printf("header_rc:%d\n", header_rc);

						std::string header_string(header_buffer);

						JSON header_json = JSON::parse(header_string);
						
						//int message_size = stoi(header_string);
						int message_size = header_json.at("message_size");

						fd_message_size[current_fd] = message_size;
						fd_read_state[current_fd] = awaiting_message;
					} //End receive header case

					else if (fd_read_state[current_fd] == awaiting_message) {
						//Receive Message Case
						printf("Receiving Message:\n");

						if (fd_message_size[current_fd] < 1) {
							printf(">>ERROR:Expected message size is not a valid value\n");
							fd_read_state[current_fd] = awaiting_header;
							fd_message_size[current_fd] = -1;
							break;
						}

						int message_size = fd_message_size[current_fd];
						char* message_buffer = new char[message_size];

						std::unique_ptr<char[]> message_buffer_uni(new char[message_size]);

						//receive message
						int message_rc = recv(fds[current_fd].fd, message_buffer, message_size, 0);

						if (message_rc < 0) {
							printf(">>ERROR:Nothing was available to read\n");
							fd_read_state[current_fd] = awaiting_header;
							fd_message_size[current_fd] = -1;
							break;
						}

						printf("message_rc:%d\n", message_rc);
						message_buffer[message_size] = 0;

						std::string message_string(message_buffer);

						fd_read_state[current_fd] = awaiting_header;
						fd_message_size[current_fd] = -1;  //set to a default value

						std::cout << "Message:" << message_string << "\n" << std::endl;

						//delete[] message_buffer;
					} //End receive message case

					else {
						printf(">>ERROR:fd_read_state[%d] is an unexpected value\n", current_fd);
						fd_read_state[current_fd] = awaiting_header;
						fd_message_size[current_fd] = -1;
						break;
					}
					//} while (true);
				} while (false);

			}  // End of existing connection is readable
		} // End of loop through pollable descriptors

		// If the close_conn flag was turned on, we to clean up this active connection. This
		// clean up process includes removing the descriptor.
		if (close_conn) {
			closesocket(fds[current_fd].fd);
			fds[current_fd].fd = -1;
			fds[current_fd].events = 0;
			fds[current_fd].revents = 0;
			compress_array = true;
		}

		// If the compress_array flag was turned on, we need to squeeze
		// together the array and decrement the number of file descriptors.
		if (compress_array) {
			compress_array = FALSE;
			for (current_fd = 0; current_fd < nfds; current_fd++) {
				if (fds[current_fd].fd == -1) {
					for (compress_index = current_fd; compress_index < nfds - 1; compress_index++) {
						fds[compress_index].fd = fds[compress_index + 1].fd;
						fd_read_state[compress_index] = fd_read_state[compress_index + 1];
						fd_message_size[compress_index] = fd_message_size[compress_index + 1];
					}
					current_fd--;
					nfds--;
				}
			}
		}
	} while (true); // End of server running.
	//} while (end_server == FALSE); // End of server running.

	// Clean up all of the sockets that are open
	for (current_fd = 0; current_fd < nfds; current_fd++) {
		if (fds[current_fd].fd >= 0)
			closesocket(fds[current_fd].fd);
	}
}


int SocketServer::initThreads() {

	//Create queue control mutex
	m_queue_control_mutex = CreateMutex(
		NULL,
		FALSE,
		NULL);

	if (m_queue_control_mutex == NULL) {
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	//Create worker threads
	for (int i = 0; i < MAX_THREADS; i++) {
		ThreadRoleArray[i].server = this;

		if (i == 0)
			ThreadRoleArray[i].role = listen_new_connection;
		else if (i == 1)
			ThreadRoleArray[i].role = receive_data;
		else
			ThreadRoleArray[i].role = work;

		hThreadArray[i] = CreateThread(
			NULL,
			0,
			staticThreadFunction,
			&(ThreadRoleArray[i]),
			0,
			&dwThreadIDArray[i]);

		if (hThreadArray[i] == NULL)
		{
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


void SocketServer::initFDSet() {
	FD_ZERO(&m_current_sockets);
	FD_SET(m_serverSocketFD, &m_current_sockets);
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


void SocketServer::listenAndAcceptIncomingConnection(int serverSocketFD) {
	std::cout << "Listening..." << std::endl;
	int listen_result = listen(m_serverSocketFD, 10);

	AcceptedSocket* new_connection = acceptIncomingConnection(m_serverSocketFD);
	m_client_list.insert(new_connection);
}


//Windows CreateThread is weird and needs this static thread function wrapper before
//  the actual thread function
DWORD WINAPI SocketServer::staticThreadFunction(void* param) {
	ThreadRoleStruct* role_info = (ThreadRoleStruct*)param;

	SocketServer* this_server = role_info->server;
	return this_server->threadFunction(&(role_info->role));
}


DWORD WINAPI SocketServer::threadFunction(ThreadRoleEnum* role) {

	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	int select_res;

	//int listen_result;
	AcceptedSocket* new_connection;

	while (true) {
		
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

		switch (*role) {
			case listen_new_connection:
				listenAndAcceptIncomingConnection(m_serverSocketFD);
				break;

			case receive_data:
				if (!m_client_list.empty()) {
					std::cout << "client list not empty" << std::endl;


					FD_ZERO(&m_current_sockets);
					FD_SET(m_serverSocketFD, &m_current_sockets);

					//copy because select is destructive
					m_ready_sockets = m_current_sockets;


					std::cout << "Waiting for select" << std::endl;
					//select_res = select(FD_SETSIZE, &m_ready_sockets, NULL, NULL, &timeout);
					select_res = select(FD_SETSIZE, &m_current_sockets, NULL, NULL, NULL);
					std::cout << "select_res:" << select_res << "\n" << std::endl;

					switch (select_res) {
						case 0:
							//Timeout case
							break;

						case -1:
							//Error case
							break;

						default:
							//Original plan was to use 1 thread for receiving new connections and data from
							//  existing connections. Hence, FDSETs were needed.
							//
							//New plan is to have 1 thread listen for new connections and a different receive
							//  so FDSETs may be put on hold for a bit
							for (int i = 0; i < FD_SETSIZE; i++) {
								//if (FD_ISSET(i, &m_ready_sockets)) {
								if (FD_ISSET(i, &m_current_sockets)) {
									if (i == m_serverSocketFD) {
										////New connection to accept
										//new_connection = acceptIncomingConnection(m_serverSocketFD);
										//m_client_list.insert(new_connection);
										//FD_SET(new_connection->m_socketFD, &m_current_sockets);
										//FD_CLR(i, &m_current_sockets);
									}
									else {
										//General client communication

										std::cout << "set socket found\n\n" << std::endl;
										AcceptedSocket* clientSocket = acceptIncomingConnection(m_serverSocketFD);
										char buffer[1024];
										int amount_received = recv(clientSocket->m_socketFD, buffer, 1024, 0);

										if (amount_received > 0) {
											buffer[amount_received] = 0;
											std::cout << buffer << "\n" << std::endl;
										}
										FD_CLR(i, &m_current_sockets);
									}
								}
							}
							break;
					}
				}
				break;
			
			case work:

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

	switch (dwWaitResult)
	{
		// The thread has ownership of the mutex
		case WAIT_OBJECT_0:
			m_work_queue.push(task);

			// Release ownership of the mutex object
			if (!ReleaseMutex(m_queue_control_mutex))
			{
				std::cout << "Mutex was not released properly." << std::endl;
			}
			break;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
		case WAIT_ABANDONED:
			return FALSE;
	}

	return 0;
}


JSON SocketServer::takeFromQueue() {
	DWORD dwWaitResult;
	JSON ret;

	dwWaitResult = WaitForSingleObject(
		m_queue_control_mutex,    // handle to mutex
		INFINITE);  // no time-out interval

	switch (dwWaitResult)
	{
		// The thread has ownership of the mutex
	case WAIT_OBJECT_0:
		//Remove task to the queue
		ret = m_work_queue.front();
		m_work_queue.pop();

		// Release ownership of the mutex object
		if (!ReleaseMutex(m_queue_control_mutex))
		{
			std::cout << "Mutex was not released properly." << std::endl;
		}
		break;

		// The thread got ownership of an abandoned mutex
		// The database is in an indeterminate state
	case WAIT_ABANDONED:
		return FALSE;
	}

	return ret;
}


void SocketServer::check(int input,std::string instance) {
	if (input == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		exit(2);
	}
}


void SocketServer::printIP() {
	char buffer[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &m_serverAddress->sin_addr, buffer, sizeof(buffer));
	std::cout << buffer << ":" << m_serverAddress->sin_port << std::endl;
}


void SocketServer::setupServerSocketFD() {
	m_serverSocketFD = createTCPIPv4Socket();

	// Allow socket descriptor to be reusable
	int on = 1;
	int rc = setsockopt(m_serverSocketFD, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&on, sizeof(on));
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

std::string SocketServer::pfdReadExistingConnection(int index) {

	return "asdf";
}