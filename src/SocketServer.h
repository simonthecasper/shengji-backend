#pragma once

#include "common.h"

#include "AcceptedSocket.h"
#include <mutex>
#include <memory>

#define MAX_THREADS		2
#define HEADER_SIZE		64
#define CLIENT_CLOSED	2
#define FD_ARRAY_SIZE	200

class SocketServer;

enum ThreadRoleEnum { listen_incoming_data, work };

enum SocketReadState { awaiting_header, awaiting_message };

struct ThreadRoleStruct {
	SocketServer* server;
	ThreadRoleEnum role;
};


class SocketServer
{

private:
	//Server IP
	SOCKET					m_serverSocketFD;
	struct sockaddr_in*		m_serverAddress;

	//Multithreading
	DWORD					dwThreadIDArray[MAX_THREADS];
	HANDLE					hThreadArray[MAX_THREADS];
	ThreadRoleStruct		ThreadRoleArray[MAX_THREADS];
	std::queue<JSON>		m_work_queue;
	HANDLE					m_queue_control_mutex;

	//Polling
	int						nfds;
	struct pollfd			fds[FD_ARRAY_SIZE];
	enum SocketReadState	fd_read_state[FD_ARRAY_SIZE];
	int						fd_message_size[FD_ARRAY_SIZE];
	bool					m_compress_fd_array;
	HANDLE					m_mutex_compress_fd_array;

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

	DWORD WINAPI threadFunction(ThreadRoleEnum* param);

	int addToQueue(JSON task);

	JSON takeFromQueue();

	void check(int input, std::string instance);

	void printIP();


	//Creates a socket for the server and configures it for Polling
	void setupServerSocketFD();

	std::string pfdReadExistingConnection(int fds_index);

	void compressFDArray();

	void closeAllSockets();

	void pollAcceptNewConnections();

	DWORD WINAPI setCompressFDArrayTrue();

	DWORD WINAPI setCompressFDArrayFalse();
};