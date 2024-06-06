#pragma once

#include "common.h"

#include "AcceptedSocket.h"
#include "SessionManager.h"

#include <memory>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <openssl/sha.h>

#define THREAD_START			0  // set to 1 to skip network listen thread
#define MAX_THREADS				2


// #define HEADER_SIZE         	64
#define HEADER_SIZE         	1024
#define FD_ARRAY_SIZE       	200
#define LISTEN_BACKLOG      	32

#define SERVER_IP				"0.0.0.0"
#define SERVER_PORT				54329

class SocketServer;

enum ThreadRoleEnum { listen_incoming_data, work };
enum SocketReadState { awaiting_header, awaiting_body };


struct ThreadRoleStruct {
	// This self referential server field is needed to find the thread function
	//   since we have the static thread function wrapper
	SocketServer* server;
	ThreadRoleEnum role;
};


class SocketServer {
private:
	// Server IP
	int						m_serverSocketFD;
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

	// SocketIO Application Server
	int m_socketio_server_fd;

	/*-------------------------------------------*/
	/*                Constructor                */
	/*-------------------------------------------*/
public:
	SocketServer();

	/*-------------------------------------------*/
	/*                  Basics                   */
	/*-------------------------------------------*/
	void runServerAsAppServer();

private:
	void initServer();

	// Creates a socket for the server and configures it for Polling
	void setupServerSocketFD();

	// Initializes all mutexes (change later??)
	void initMutex();

	// Prints server socket IP information
	void printIP();


	/*-------------------------------------------*/
	/*        Poll and Socket Connections        */
	/*-------------------------------------------*/

	// Runs the basic poll loop to read incoming data from sockets.
	// This is the work function of the thread that is assigned the
	//	 listen_incoming_data role
	void pollSocketArrayAppServer();

	// Calls poll with the provided timeout value and returns the poll result.
	// Exits the program if poll fails.
	int waitForPoll(int timeout);

	// Adds new socket connections as detected by poll to the FD array
	void pollAcceptNewConnections();

	// Accepts an incoming socket sonnection at the provided socketFD
	AcceptedSocket* acceptIncomingConnection(int serverSocketFD);

	// Reads the socketFD at the provided fd_index in the m_pollfd_array and processes 
	// whatever the message needs.
	//  -Received Header: Prepares for message body
	//  -Received Body: Processes body and prepares for next message header
	//  -Received Socket Close: Safely closes socket and signals so
	void pollReceiveAndProcessMessage(int fd_index);

	// Returns a structaddr_in* at the provided IPv4 address and port
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


	/*-------------------------------------------*/
	/*        Multithreading / ThreadPool        */
	/*-------------------------------------------*/

	void initThreadsAppServer();

	// Static thread function wrapper to get rid of the hidden "this" parameter
	static void* staticThreadFunctionAppServer(void* args);

	// Thread function that does work depending on the role given to the thread.
	// Threads have 2 possible roles:
	//	--listen_incoming_data: reads if there is any new data from sockets.
	//	 	Only 1 thread will be assigned to this role.
	//  --work: retrieves work from the threadpool queue and does whatever
	//		that task needs.
	void* threadFunctionAppServer(ThreadRoleEnum role);

	int addToQueue(JSON task);

	// Thread safe function that retrieves a task from the work queue if available
	JSON getWorkFromQueue();
};
