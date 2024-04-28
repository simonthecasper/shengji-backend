#pragma once

#include <iostream>
#include <list>
#include <Windows.h>
#include "json.hpp"

using JSON = nlohmann::json;

class Chat
{
private:
	std::list<JSON> m_chatlog;
	HANDLE		m_mutex_chatlog;

public:
	Chat();

	int addToChat(int player_id, std::string message);
};

