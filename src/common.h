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

#include <chrono>

#include "../include-3p/json.hpp"


using JSON = nlohmann::json;

typedef unsigned char BYTE;
typedef std::chrono::milliseconds time_ms;

namespace common {
	static std::mutex	m_socket_send_mutex;
	static int 			m_socketio_server;

	static time_ms 		m_start_time;
	static std::mutex 	m_time_mutex;

	static std::vector<std::string> ProgressCards = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };

	int sendThroughSocket(int destination, JSON message_json);

	int sendThroughSocket(int destination, std::string message_str);

	int sendToMiddleman(JSON message_json);

	int sendThroughSocketSID(JSON message_json);

	int sendThroughSocketSID(std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);

	void print(std::string s);

	void setSocketIOServerFD(int fd);

	void setStartTime();

	time_ms getTime();

	bool isProgressCard(std::string card);
}