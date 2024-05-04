#pragma once

#include "common.h"

#include "AcceptedSocket.h"
#include "SessionManager.h"

#include <mutex>
#include <memory>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MAX_THREADS				3

#define HEADER_SIZE         	64
#define FD_ARRAY_SIZE       	200
#define LISTEN_BACKLOG      	32

#define CLIENT_CLOSED_PY_LINUX 	1
#define CLIENT_CLOSED       	2
#define CLIENT_CLOSED_PY    	3

// #define SERVER_IP				"192.168.0.77"
// #define SERVER_IP				"172.19.128.1"
#define SERVER_IP				"0.0.0.0"
#define SERVER_PORT				12345

class SocketServer;

enum ThreadRoleEnum { listen_incoming_data, work };
enum SocketReadState { awaiting_header, awaiting_body };


struct ThreadRoleStruct {
	// This self referential server field is needed to find the thread function
	//   since we have the static thread function wrapper
	SocketServer* server;
	ThreadRoleEnum role;
};


class SocketServer
{

private:
	// Server IP
	int					m_serverSocketFD;
	struct sockaddr_in* m_serverAddress;

	// Multithreading/ThreadPool
	uint					m_threadIDArray[MAX_THREADS];
	pthread_t				m_thread_pool[MAX_THREADS];
	ThreadRoleStruct		m_thread_role_array[MAX_THREADS];
	std::queue<JSON>		m_work_queue;
	std::mutex				m_queue_control_mutex;

	// Polling
	int						m_nfds;
	struct pollfd			m_pollfd_array[FD_ARRAY_SIZE];
	SocketReadState			m_fd_read_state[FD_ARRAY_SIZE];
	int						m_fd_message_size[FD_ARRAY_SIZE];
	bool					m_compress_fd_array;
	std::mutex				m_mutex_compress_flag;
	std::mutex				m_mutex_fd_array;

	// Session Managing
	SessionManager* m_session_manager;


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

	//void check(int input, std::string instance);

	void printIP();


	/*-------------------------------------------*/
	/*        Poll and Socket Connections        */
	/*-------------------------------------------*/
	void pollSocketArray();

	// Calls WSAPoll with the provided timeout value and returns the poll result.
	//   Exits the program if poll fails.
	int waitForPoll(int timeout);

	// Adds new socket connections as detected by poll to the FD array
	void pollAcceptNewConnections();

	// Accepts an incoming socket sonnection at the provided socketFD
	AcceptedSocket* acceptIncomingConnection(int serverSocketFD);

	// Receives an available message header from the provided socket FD
	std::string pollReceiveMessageHeader(int index);

	// Receives an available message body from the provided socket FD
	std::string pollReceiveMessageBody(int current_fd);

	struct sockaddr_in* createIPv4Address(std::string ip, int port);

	// Creates and returns a socketFD
	int createTCPIPv4Socket();

	// Removes unused index spaces in the FD array after connections are closed
	void compressFDArray();

	// Thread safe function that sets the compressFDArray flag to true
	bool setCompressFDArrayTrue();

	// Thread safe function that sets the compressFDArray flag to false
	bool setCompressFDArrayFalse();

	// Closes the socket connection to the socket in the provided index
	bool closeConnectionFDArray(int current_fd);

	// Closes all open sockets in the socket FD array
	void closeAllSockets();

	// Sends the message to all connected sockets. Used for testing
	void testSendToAllOthers(int source, std::string message);

	// Returns true if the client has closed their connection
	// Returns false if not
	bool checkIfClientClosed(int fdarray_index);

	/*-------------------------------------------*/
	/*        Multithreading / ThreadPool        */
	/*-------------------------------------------*/

	// Initializes all threads and assigns them their respective roles
	int initThreads();

	// Static thread function wrapper to get rid of the hidden "this" parameter
	static void* staticThreadFunction(void* args);

	// Actual thread function
	void* threadFunction(ThreadRoleEnum role);

	// Thread safe function that adds a task to the work queue
	int addToQueue(JSON task);

	// Thread safe function that retrieves a task from the work queue if available
	JSON getWorkFromQueue();
};