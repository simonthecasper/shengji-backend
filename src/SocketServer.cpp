#include "SocketServer.h"

#define SERVER_PORT 2000

SocketServer::SocketServer() {
	initServer();
}

SOCKET SocketServer::createTCPIPv4Socket() {
	SOCKET a = socket(AF_INET, SOCK_STREAM, 0);
	return a;
}


void SocketServer::initServer() {  //copying main
	m_serverSocketFD = createTCPIPv4Socket();
	m_serverAddress = createIPv4Address("", SERVER_PORT);

	int bind_result = bind(m_serverSocketFD, (struct sockaddr*)m_serverAddress, sizeof(*m_serverAddress));
	std::cout << bind_result << "\n" << std::endl;
	check(bind_result, "Server bind");
	std::cout << "Server socket bound successfullly" << std::endl;
	
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


struct sockaddr_in* SocketServer::createIPv4Address(String ip, int port) {
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



void* threadFunction(void* arg) {
	while (true) {
		
	}
}


void SocketServer::check(int input, String instance) {
	if (input == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		exit(2);
	}
}