#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "AcceptedSocket.h"
#include <iostream>
#include <queue>

//#define THREAD_POOL_SIZE 10
//
//HANDLE ThreadArray[THREAD_POOL_SIZE];

class SocketServer
{
private:
	SOCKET m_serverSocketFD;
	struct sockaddr_in* m_serverAddress;

public:
	SocketServer();

	void initServer();

	AcceptedSocket* acceptIncomingConnection(int serverSocketFD);

	struct sockaddr_in* createIPv4Address(String ip, int port);
	
	SOCKET createTCPIPv4Socket();

	//void* threadFunction(void* arg);

	void check(int input, String instance);
};