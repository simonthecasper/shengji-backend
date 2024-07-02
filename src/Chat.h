#pragma once

#include <iostream>
#include <list>
#include <mutex>

#include "common.h"

using JSON = nlohmann::json;


class Chat
{
private:
	std::list<std::pair<std::string, std::string>> m_chatlog;
	std::mutex		m_mutex_chatlog;

public:
	Chat();

	int addToChat(std::string player_id, std::string message);

	void handleMessage(JSON message);
};

