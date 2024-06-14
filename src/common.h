#pragma once


#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <errno.h>
#include <unistd.h>

#include <time.h>

#include "../include-3p/json.hpp"


using JSON = nlohmann::json;

typedef unsigned char BYTE;

namespace common {

	static std::mutex m_socket_send_mutex;
	static int m_socketio_server;

	static time_t m_start_time;
	static std::mutex m_time_mutex;


	int sendThroughSocket(int destination, JSON message_json);

	int sendThroughSocket(int destination, std::string message_str);

	int sendThroughSocketSID(JSON message_json);

	int sendThroughSocketSID(std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);

	void print(std::string s);

	void setSocketIOServerFD(int fd);

	void setStartTime();

	double getTime();

}