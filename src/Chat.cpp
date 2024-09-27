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


void Chat::handleMessage(JSON message) {
	common::print("In Chat handler...");
	std::string task = message.at("task");

	if (common::stringCompare(task, "send_chat")) {
		std::string player_id = message.at("player_id");
		std::string message_text = message.at("message");
		addToChat(player_id, message_text);
	}
}