#include "Chat.h"


Chat::Chat() {
	m_mutex_chatlog.unlock();
}


int Chat::addToChat(std::string player_id, std::string message) {
	m_mutex_chatlog.lock();

	JSON message_json;
	message_json["player_id"] = player_id;
	message_json["message"] = message;

	m_chatlog.push_back(message_json);

	m_mutex_chatlog.unlock();
	return 0;
}
