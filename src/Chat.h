#pragma once

#include <iostream>
#include <list>
#include <mutex>

#include "common.h"

using JSON = nlohmann::json;


class Chat
{
private:
	//change from JSON to std::pair??
	std::list<JSON> m_chatlog;
	std::mutex		m_mutex_chatlog;

public:
	Chat();

	int addToChat(std::string player_id, std::string message);
};

