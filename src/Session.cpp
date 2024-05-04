#include "Session.h"



Session::Session(std::string id) {
	m_id = id;
	m_chatlog = new Chat();
	m_player_count = 0;
}


void Session::addToChat(JSON message_json) {
	int player_id = message_json.at("player_id");
	std::string message = message_json.at("message");
	m_chatlog->addToChat(player_id, message);

	sendToOtherPlayers(player_id, message_json);
}

int Session::addPlayer(int player_fd) {
	int id = generateID();
	Player* p = new Player(id);

	m_player_ids.insert(id);
	m_player_list.insert(p);
	m_player_sockets.insert(player_fd);

	m_player_count = (int)m_player_list.size();
	m_id_to_socket[id] = player_fd;
	m_socket_to_id[player_fd] = id;
	m_id_to_player[id] = p;

	return id;
}

int Session::generateID() {
	int id = rand() % 10000;
	while (m_player_ids.count(id) != 0)
		id = rand() % 10000;
	return id;
}

int Session::handleJSON(JSON input) {

	std::string stage = input.at("stage");

	if (strcmp(stage.c_str(), "chat") == 0) {

	}

	return 0;
}

std::string Session::getID() {
	return m_id;
}

void Session::sendToOtherPlayers(int source_player, JSON message) {
	int source_fd = m_id_to_socket.at(source_player);

	unordered_set<int>::iterator itr;
	// Displaying set elements
	for (itr = m_player_sockets.begin(); itr != m_player_sockets.end(); itr++) {
		if (*itr != source_fd) {
			common::sendThroughSocket(*itr, message.dump());
		}
	}
}

void Session::removePlayer(int socket) {
	int player_id = m_socket_to_id.at(socket);
	Player* target = m_id_to_player.at(player_id);

	m_player_list.erase(target);
	m_player_ids.erase(player_id);
	m_player_sockets.erase(socket);
	m_id_to_socket.erase(player_id);
	m_socket_to_id.erase(socket);
	m_id_to_player.erase(player_id);

	m_player_count--;
}