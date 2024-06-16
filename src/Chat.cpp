#include "Chat.h"


Chat::Chat() {
	m_mutex_chatlog.unlock();
}


int Chat::addToChat(std::string player_id, std::string message) {
	m_mutex_chatlog.lock();

	std::pair<std::string, std::string> entry = std::make_pair(player_id, message);
	m_chatlog.push_back(entry);

	m_mutex_chatlog.unlock();
	return 0;
}
