#pragma once


#include <algorithm>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "json.hpp"


using JSON = nlohmann::json;

typedef unsigned char BYTE;


namespace common {

	int sendThroughSocket(int destination, JSON message_json);

	int sendThroughSocket(int destination, std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);

	std::string base64_encode(std::string input);

	std::string base64_encode(const unsigned char* buf, int bufLen);

	std::string base64_decode(std::string const&);

}