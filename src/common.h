#pragma once

#include <iostream>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "json.hpp"

using JSON = nlohmann::json;


namespace common {

	int sendThroughSocket(int destination, JSON message_json);

	int sendThroughSocket(int destination, std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);
}