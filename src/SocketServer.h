#pragma once

#include "common.h"

#include "AcceptedSocket.h"
#include <mutex>
#include <memory>

#define MAX_THREADS		3

#define HEADER_SIZE		64
#define CLIENT_CLOSED	2
#define FD_ARRAY_SIZE	200
#define LISTEN_BACKLOG	32

class SocketServer;

enum ThreadRoleEnum { listen_incoming_data, work };
enum SocketReadState { awaiting_header, awaiting_body };

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
	HANDLE					m_mutex_compress_flag;
	HANDLE					m_mutex_fd_array;

	//Session handling
	std::unordered_set<AcceptedSocket*> m_client_list;

public:
	SocketServer();

	void initServer();

	void initMutex();

	void pollSocketArray();

	int initThreads();

	void initFDSet();

	struct sockaddr_in* createIPv4Address(std::string ip, int port);
	
	SOCKET createTCPIPv4Socket();
	
	AcceptedSocket* acceptIncomingConnection(SOCKET serverSocketFD);

	void listenAndAcceptIncomingConnection(int serverSocketFD);

	static DWORD WINAPI staticThreadFunction(void* param);

	DWORD WINAPI threadFunction(ThreadRoleEnum* param);

	int addToQueue(JSON task);

	JSON removeFromQueue();

	JSON getWorkFromQueue();

	void check(int input, std::string instance);

	void printIP();

	//Creates a socket for the server and configures it for Polling
	void setupServerSocketFD();

	void compressFDArray();

	void closeAllSockets();

	int waitForPoll(int timeout);

	void pollAcceptNewConnections();

	std::string pollReceiveMessageHeader(int index);

	std::string pollReceiveMessageBody(int current_fd);

	bool setCompressFDArrayTrue();

	bool setCompressFDArrayFalse();

	bool closeConnectionFDArray(int current_fd);
};