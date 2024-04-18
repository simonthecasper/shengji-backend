#pragma once

#include "common.h"

#include "AcceptedSocket.h"
#include <mutex>



#define MAX_THREADS 2

class SocketServer;

enum ThreadRoleEnum { listen_new_connection, receive_data, work };

struct ThreadRoleStruct {
	SocketServer* server;
	ThreadRoleEnum role;
};


class SocketServer
{

private:
	//Server IP
	SOCKET				m_serverSocketFD;
	struct sockaddr_in* m_serverAddress;

	//Multithreading
	DWORD				dwThreadIDArray[MAX_THREADS];
	HANDLE				hThreadArray[MAX_THREADS];
	ThreadRoleStruct	ThreadRoleArray[MAX_THREADS];
	std::queue<JSON>	m_work_queue;
	HANDLE				m_queue_control_mutex;
	
	FD_SET				m_current_sockets;
	FD_SET				m_ready_sockets;

	//Session handling
	std::unordered_set<AcceptedSocket*> m_client_list;

public:
	SocketServer();

	void initServer();


	void pollTest();


	int initThreads();

	void initFDSet();

	struct sockaddr_in* createIPv4Address(std::string ip, int port);
	
	SOCKET createTCPIPv4Socket();
	
	AcceptedSocket* acceptIncomingConnection(SOCKET serverSocketFD);

	void listenAndAcceptIncomingConnection(int serverSocketFD);

	static DWORD WINAPI staticThreadFunction(void* param);

	//DWORD WINAPI threadFunction(ThreadRoleEnum* param);
	DWORD WINAPI threadFunction(ThreadRoleEnum* param);


	int addToQueue(JSON task);

	JSON takeFromQueue();

	void check(int input, std::string instance);

	void printIP();
};