#pragma once

#include<iostream>
#include<string>
#include<list>
#include<unordered_map>
#include<unordered_set>
#include<queue>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <strsafe.h>
#include <tchar.h>

#include "json.hpp"

using JSON = nlohmann::json;


namespace common {

	int sendThroughSocket(SOCKET destination, JSON message_json);

	int sendThroughSocket(SOCKET destination, std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);
}

