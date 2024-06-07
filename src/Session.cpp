#include "Session.h"


Session::Session(std::string id) {
	m_id = id;
	m_chatlog = new Chat();
	m_player_count = 0;
}

void handleMessage(JSON message_json) {

}

void Session::addToChat(JSON message_json) {
	std::string player_id = message_json.at("player_id");
	std::string message = message_json.at("message");
	m_chatlog->addToChat(player_id, message);

	std::string player_sid = message_json.at("sid");
	sendToOtherPlayersSID(player_sid, message_json);
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

std::string Session::addPlayerSID(std::string sid) {
	std::string player_id = generatePlayerID();
	Player* p = new Player(player_id);

	m_player_ids.insert(player_id);
	m_player_list.insert(p);
	m_player_sids.insert(sid);

	m_player_count = (int)m_player_list.size();
	m_id_to_sid[player_id] = sid;
	m_sid_to_id[sid] = player_id;
	m_id_to_player[player_id] = p;

	return player_id;
}

void Session::removePlayerSID(std::string sid) {
	std::string player_id = m_sid_to_id.at(sid);
	Player* target = m_id_to_player.at(player_id);

	m_player_list.erase(target);
	m_player_ids.erase(player_id);
	m_player_sids.erase(sid);
	m_sid_to_id.erase(sid);
	m_id_to_player.erase(player_id);

	m_player_count--;
}

std::string Session::generatePlayerID() {
	std::string random_string = "";
	do {
		const string CHARACTERS = "abcdefghijklmnopqrstuv";

		random_device rd;
		mt19937 generator(rd());
		uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

		random_string = "p_";
		for (int i = 0; i < PLAYER_ID_LENGTH - 2; ++i) {
			random_string += CHARACTERS[distribution(generator)];
		}
	} while (m_player_ids.find(random_string) == m_player_ids.end());
	//Above means while random_string not in m_player_ids
	//Perhaps make common::setContains function (and mapContainsKey, etc.)

	return random_string;
}

std::string Session::getID() {
	return m_id;
}
