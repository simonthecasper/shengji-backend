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
	//this self referential server field is needed to find the thread function
	//since we have the static thread function wrapper
	SocketServer* server;
	ThreadRoleEnum role;
};


class SocketServer
{

private:
	// Server IP
	SOCKET					m_serverSocketFD;
	struct sockaddr_in*		m_serverAddress;

	// Multithreading/ThreadPool
	DWORD					dwThreadIDArray[MAX_THREADS];
	HANDLE					hThreadArray[MAX_THREADS];
	ThreadRoleStruct		ThreadRoleArray[MAX_THREADS];
	std::queue<JSON>		m_work_queue;
	HANDLE					m_queue_control_mutex;

	// Polling
	int						m_nfds;
	struct pollfd			m_pollfd_array[FD_ARRAY_SIZE];
	enum SocketReadState	m_fd_read_state[FD_ARRAY_SIZE];
	int						m_fd_message_size[FD_ARRAY_SIZE];
	bool					m_compress_fd_array;
	HANDLE					m_mutex_compress_flag;
	HANDLE					m_mutex_fd_array;

	// Session handling
	std::unordered_set<AcceptedSocket*> m_client_list;

public:
	SocketServer();

private:
	/*-------------------------------------------*/
	/*                  Basics                   */
	/*-------------------------------------------*/
	void initServer();

	//Creates a socket for the server and configures it for Polling
	void setupServerSocketFD();

	//Initializes all mutexes (change later??)
	void initMutex();

	void check(int input, std::string instance);

	void printIP();

	/*-------------------------------------------*/
	/*        Poll and Socket Connections        */
	/*-------------------------------------------*/
	void pollSocketArray();

	// Adds new socket connections as detected by poll to the FD array
	void pollAcceptNewConnections();

	// Accepts an incoming socket sonnection at the provided socketFD
	AcceptedSocket* acceptIncomingConnection(SOCKET serverSocketFD);

	// Receives an available message header from the provided socket FD
	std::string pollReceiveMessageHeader(int index);

	// Receives an available message body from the provided socket FD
	std::string pollReceiveMessageBody(int current_fd);

	// Calls WSAPoll with the provided timeout value and returns the poll result.
	//   Exits the program if poll fails.
	int waitForPoll(int timeout);

	struct sockaddr_in* createIPv4Address(std::string ip, int port);
	
	SOCKET createTCPIPv4Socket();

	// Removes unused index spaces in the FD array after connections are closed
	void compressFDArray();

	bool setCompressFDArrayTrue();

	bool setCompressFDArrayFalse();

	bool closeConnectionFDArray(int current_fd);

	// Closes all open sockets in the socket FD array
	void closeAllSockets();

	/*-------------------------------------------*/
	/*        Multithreading / ThreadPool        */
	/*-------------------------------------------*/
	int initThreads();

	static DWORD WINAPI staticThreadFunction(void* param);

	DWORD WINAPI threadFunction(ThreadRoleEnum* param);

	//Thread safe function that adds a task to the work queue
	int addToQueue(JSON task);

	//Thread safe function that retrieves a task from the work queue if available
	JSON getWorkFromQueue();
};