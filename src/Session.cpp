#include "Session.h"



Session::Session(std::string id) {
	m_id = id;
	m_chatlog = new Chat();
	m_player_count = 0;
}


/*-------------------------------------------*/
/*              Python Client                */
/*-------------------------------------------*/

void Session::addToChat(JSON message_json) {
	int player_id = message_json.at("player_id");
	std::string message = message_json.at("message");
	m_chatlog->addToChat(player_id, message);

	std::string player_sid = message_json.at("sid");
	sendToOtherPlayersSID(player_sid, message_json);
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


void Session::sendToOtherPlayersSID(std::string source_sid, JSON message) {
	unordered_set<std::string>::iterator itr;
	// Displaying set elements
	for (itr = m_player_sids.begin(); itr != m_player_sids.end(); itr++) {
		if (*itr != source_sid) {
			common::sendThroughSocketSID(message.dump());
		}
	}
}


int Session::addPlayerSID(std::string sid) {
	int id = generateID();
	Player* p = new Player(id);

	m_player_ids.insert(id);
	m_player_list.insert(p);
	m_player_sids.insert(sid);

	m_player_count = (int)m_player_list.size();
	m_id_to_sid[id] = sid;
	m_sid_to_id[sid] = id;
	m_id_to_player[id] = p;

	return id;
}

void Session::removePlayerSID(std::string sid) {
	int player_id = m_sid_to_id.at(sid);
	Player* target = m_id_to_player.at(player_id);

	m_player_list.erase(target);
	m_player_ids.erase(player_id);
	m_player_sids.erase(sid);
	m_id_to_socket.erase(player_id);
	m_sid_to_id.erase(sid);
	m_id_to_player.erase(player_id);

	m_player_count--;
}