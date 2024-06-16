#include "Session.h"

Session::Session() {

}

Session::Session(std::string id) {
	m_id = id;
	m_chatlog = new Chat();
	m_player_count = 0;
}

void Session::handleMessage(JSON message) {
	std::string stage = message.at("stage");
	std::string task = message.at("task");

	if (common::stringCompare(task, "chat")) {
		addToChat(message);
	}

	else if (common::stringCompare(task, "lobby")) {
		if (common::stringCompare(task, "new_session")) {
			std::string sid = message.at("sid");
			std::string username = message.at("username");
			addPlayer(sid, username);
		}

		else if (common::stringCompare(task, "join_session")) {
			std::string sid = message.at("sid");
			std::string username = message.at("username");
			addPlayer(sid, username);
		}

		else if (common::stringCompare(task, "player_set_team")) {
			std::string player_id = message.at("player_id");
			std::string team = message.at("team");
			addPlayerToTeam(player_id, team);
		}

		else if (common::stringCompare(task, "start_game")) {

		}
	}

	else if (common::stringCompare(task, "draw") || common::stringCompare(task, "game")) {
		m_current_game.handleMessage(message);
	}

}

void Session::addToChat(JSON message_json) {
	std::string player_id = message_json.at("player_id");
	std::string message = message_json.at("message");
	m_chatlog->addToChat(player_id, message);

	S2CMessages::sendBroadcastChat(m_player_list, player_id, message);
}

void Session::sendToOtherPlayersSID(std::string source_sid, JSON message) {
	std::unordered_set<std::string>::iterator itr;
	// Displaying set elements
	for (itr = m_player_sids.begin(); itr != m_player_sids.end(); itr++) {
		if (*itr != source_sid) {
			common::sendThroughSocketSID(message.dump());
		}
	}
}

void Session::addPlayer(std::string sid, std::string username) {
	// Players SID already exists in this session
	if (sidInSession(sid))
		return;

	std::string player_id = generatePlayerID();
	Player* p = new Player(player_id, username, sid);


	m_player_ids.insert(player_id);
	m_player_list.push_back(p);
	m_player_sids.insert(sid);

	m_player_count = (int)m_player_list.size();
	m_id_to_sid[player_id] = sid;
	m_sid_to_id[sid] = player_id;
	m_id_to_player[player_id] = p;

	m_player_teams[player_id] = "Unset";

	S2CMessages::sendJoinSessionAck(sid, m_id, player_id);
	S2CMessages::sendShareLobbyInfo(m_player_list, m_host_player->getID(), player_id);
	S2CMessages::sendBroadcastNewPlayer(m_player_list, player_id, username);
}

void Session::removePlayerSID(std::string sid) {
	std::string player_id = m_sid_to_id.at(sid);
	Player* target = m_id_to_player.at(player_id);

	m_player_list.remove(target);

	m_player_ids.erase(player_id);
	m_player_sids.erase(sid);
	m_sid_to_id.erase(sid);
	m_id_to_player.erase(player_id);

	m_player_count--;
}

std::string Session::generatePlayerID() {
	std::string random_string = "";
	do {
		const std::string CHARACTERS = "abcdefghijklmnopqrstuv";

		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

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


void Session::addPlayerToTeam(std::string player_id, std::string team) {
	m_set_team_mutex.lock();
	m_id_to_player[player_id]->setTeam(team);
	m_player_teams[player_id] = team;
	m_set_team_mutex.unlock();

	S2CMessages::sendBroadcastTeamUpdate(m_player_list, m_player_teams);
}

bool Session::sidInSession(std::string sid) {
	return m_player_sids.find(sid) != m_player_sids.end();
}

void Session::setHostPlayer(std::string player_id) {
	m_host_player = m_id_to_player[player_id];
}


void Session::checkAndCreateGame() {

}

bool Session::checkIfLobbyReady() {
	//Must have enough players and must be even count
	if (m_player_count < 4 || m_player_count % 2 == 1) { return false; }

	//Teams must be evenly distributed and no one can be unset
	std::pair<int, int> count = std::make_pair(0, 0);
	for (const auto& player : m_player_teams) {
		if (common::stringCompare(player.second, "Team1")) {
			count.first++;
		}
		else if (common::stringCompare(player.second, "Team2")) {
			count.second++;
		}
		else if (common::stringCompare(player.second, "Unset")) {
			std::string message = "All players must be assigned to a team.";
			S2CMessages::sendLobbyNotReady(m_host_player->getSID(), message);
			return false;
		}
		else {
			std::string message = "ERROR:: Unknown value in team assignment structure";
			S2CMessages::sendLobbyNotReady(m_host_player->getSID(), message);
			return false;
		}
	}

	if (count.first != count.second) { return false; }

	return true;
}