#include "SocketServer.h"

#define SERVER_PORT 2000

SocketServer::SocketServer() {

	initServer();

	initThreads();
}


SOCKET SocketServer::createTCPIPv4Socket() {
	SOCKET a = socket(AF_INET, SOCK_STREAM, 0);
	return a;
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
		ThreadRoleArray[i].role = (i == 0 ? receive : work);

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
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// Close thread and mutex handles
	for (int i = 0; i < MAX_THREADS; i++)
		CloseHandle(hThreadArray[i]);

	CloseHandle(m_queue_control_mutex);

	return 0;
}


void SocketServer::initServer() {  //copying main
	m_serverSocketFD = createTCPIPv4Socket();
	m_serverAddress = createIPv4Address("", SERVER_PORT);

	int bind_result = bind(m_serverSocketFD, (struct sockaddr*)m_serverAddress, sizeof(*m_serverAddress));
	std::cout << bind_result << "\n" << std::endl;
	check(bind_result, "Server bind");
	std::cout << "Server socket bound successfullly" << std::endl;

	FD_ZERO(&m_current_sockets);
	FD_SET(m_serverSocketFD, &m_current_sockets);
}


AcceptedSocket* SocketServer::acceptIncomingConnection(int serverSocketFD) {
	SOCKADDR_IN clientAddress;
	int clientAddressSize = sizeof(clientAddress);
	SOCKET clientSocketFD = accept(serverSocketFD, (sockaddr*)&clientAddress, &clientAddressSize);
	
	AcceptedSocket* acceptedSocket = new AcceptedSocket();
	acceptedSocket->m_address = clientAddress;
	acceptedSocket->m_socketFD = clientSocketFD;
	acceptedSocket->m_accepted_successful = clientSocketFD > 0;

	if (!acceptedSocket->m_accepted_successful) {
		acceptedSocket->m_error = clientSocketFD;
	}

	return acceptedSocket;
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


//Windows CreateThread is weird and needs this static thread function wrapper before
//  the actual thread function
DWORD WINAPI SocketServer::staticThreadFunction(void* param) {
	ThreadRoleStruct* role_info = (ThreadRoleStruct*)param;

	SocketServer* this_server = role_info->server;
	return this_server->threadFunction(&(role_info->role));
}

DWORD WINAPI SocketServer::threadFunction(void* param) {
	ThreadRoleStruct* role_info = (ThreadRoleStruct*)param;

	while (true) {
		switch (role_info->role) {
			case receive:
				//copy because select is destructive
				m_ready_sockets = m_current_sockets;

				if (select(FD_SETSIZE, &m_ready_sockets, NULL, NULL, NULL) < 0) {
					perror("Select error");
					exit(EXIT_FAILURE);
				}

				for (int i = 0; i < FD_SETSIZE; i++) {
					if (FD_ISSET(i, &m_ready_sockets)) {
						if (i == m_serverSocketFD) {
							//New connection to accept
							AcceptedSocket* new_connection = new AcceptedSocket;
							new_connection = acceptIncomingConnection(m_serverSocketFD);
							m_client_list.insert(new_connection);
							FD_SET(new_connection->m_socketFD, &m_current_sockets);
						}
						else {
							//General client communication
						}
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