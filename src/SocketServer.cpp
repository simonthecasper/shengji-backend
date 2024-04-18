#include "SocketServer.h"

#define SERVER_PORT 2000


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


void SocketServer::initServer() {  //copying main
	m_serverSocketFD = createTCPIPv4Socket();

	/*************************************************************/
	/* Allow socket descriptor to be reuseable                   */
	/*************************************************************/
	int on = 1;
	int rc = setsockopt(m_serverSocketFD, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&on, sizeof(on));
	if (rc < 0) {
		perror("setsockopt() failed");
		closesocket(m_serverSocketFD);
		exit(-1);
	}

	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for      */
	/* the incoming connections will also be nonblocking since   */
	/* they will inherit that state from the listening socket.   */
	/*************************************************************/
	u_long on2 = 1;
	rc = ioctlsocket(m_serverSocketFD, FIONBIO, &on2);
	if (rc < 0) {
		perror("ioctl() failed");
		closesocket(m_serverSocketFD);
		exit(-1);
	}
	


	//m_serverAddress = createIPv4Address("", SERVER_PORT);
	m_serverAddress = createIPv4Address("127.0.0.1", SERVER_PORT);

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
	char   buffer[80];
	struct sockaddr_in6   addr;
	int    timeout;
	struct pollfd fds[200];
	int    nfds = 1, current_size = 0, i, j;
	AcceptedSocket* new_connection;

	/*************************************************************/
	/* Set the listen back log                                   */
	/*************************************************************/
	rc = listen(m_serverSocketFD, 32);
	if (rc < 0) {
		perror("listen() failed");
		closesocket(m_serverSocketFD);
		exit(-1);
	}

	memset(fds, 0, sizeof(fds));

	fds[0].fd = m_serverSocketFD;
	fds[0].events = POLLIN;
	
	timeout = -1;
	//timeout = (3 * 60 * 1000);
	int poll_result;
	do {
		printf("Waiting on poll()...\n");
		poll_result = WSAPoll(fds, nfds, timeout);
		
		//SOCKET_ERROR
		if (poll_result == SOCKET_ERROR) {
			perror("  poll() failed with SOCKET_ERROR");
			break;
		}
		
		//timeout case
		else if (poll_result == 0) {
			printf("  poll() timed out.  End program.\n");
			break;
		}


		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		current_size = nfds;
		for (i = 0; i < current_size; i++) {
			if (fds[i].revents == 0)
				continue;

			/************************************************************/
			/* If revents is not "Read ready", it's an unexpected result,*/
			/* log and end the server.                                  */
			/************************************************************/
			if ((fds[i].revents & POLLIN) == 0) {
				printf("  revents is not a \"Read available\" value\n");
				printf("  revents = %d\n\n", fds[i].revents);
				end_server = TRUE;
				break;
			}


			if (fds[i].fd == m_serverSocketFD) {
				printf("  Listening socket is readable\n");

				do {
					/*****************************************************/
					/* Accept each incoming connection. If               */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other              */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					/*****************************************************/

					new_connection = acceptIncomingConnection(m_serverSocketFD);

					//if (new_connection->m_socketFD  != INVALID_SOCKET) {
					if (new_connection != NULL) {
						/*****************************************************/
						/* Add the new incoming connection to the            */
						/* pollfd structure                                  */
						/*****************************************************/
						new_sd = new_connection->m_socketFD;
						printf("  New incoming connection : %d\n", new_sd);
						fds[nfds].fd = new_sd;
						fds[nfds].events = POLLIN;
						nfds++;
					}

					//CURRENT ERROR
					//  m_socketFD is an unsigned int64. Therefore, it will never be < 0 as checked
					//  on line 147.
					//	The current code uses that check to know if the connection backlog has finished or not
				} while (new_connection != NULL);
			}

			/*********************************************************/
			/* This is not the listening socket, therefore an        */
			/* existing connection must be readable                  */
			/*********************************************************/

			else {
				std::cout << "  Descriptor " << fds[i].fd << " is readable\n" << std::endl;
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do {
					/*****************************************************/
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					/*****************************************************/
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  recv() failed");
							close_conn = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					len = rc;
					printf("  %d bytes received\n", len);

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					rc = send(fds[i].fd, buffer, len, 0);
					if (rc < 0)
					{
						perror("  send() failed");
						close_conn = TRUE;
						break;
					}

				} while (TRUE);

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn)
				{
					closesocket(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}


			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for (j = i; j < nfds - 1; j++)
					{
						fds[j].fd = fds[j + 1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	} while (true); /* End of serving running.    */
	//} while (end_server == FALSE); /* End of serving running.    */

	/*************************************************************/
	/* Clean up all of the sockets that are open                 */
	/*************************************************************/
	for (i = 0; i < nfds; i++)
	{
		if (fds[i].fd >= 0)
			closesocket(fds[i].fd);
	}
}


int SocketServer::initThreads() {

	//Create queue control mutex
	m_queue_control_mutex = CreateMutex(
		NULL,
		FALSE,
		NULL);

	if (m_queue_control_mutex == NULL)
	{
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

		std::cout << "New connection:" << acceptedSocket->toString() << std::endl;
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