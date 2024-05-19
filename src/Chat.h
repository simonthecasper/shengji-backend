#pragma once

#include <iostream>
#include <list>
#include <mutex>

#include "common.h"

using JSON = nlohmann::json;


class Chat
{
private:
	std::list<JSON> m_chatlog;
	std::mutex		m_mutex_chatlog;

public:
	Chat();

	int addToChat(int player_id, std::string message);
};

