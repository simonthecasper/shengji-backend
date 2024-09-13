#include "Session.h"

Session::Session() {

}

Session::Session(std::string id) {
	m_id = id;
	m_state = lobby;
	m_chatlog = new Chat();
	m_player_count = 0;
	m_current_game = nullptr;
}

void Session::handleMessage(JSON message) {
	common::print("In Session handler...");
	std::string stage = message.at("stage");
	std::string task = message.at("task");

	if (common::stringCompare(task, "chat")) {
		m_chatlog->handleMessage(message);
	}

	else if (common::stringCompare(stage, "prelobby")) {
		if (common::stringCompare(task, "new_session") || common::stringCompare(task, "join_session")) {
			common::print("Adding player to new session...");
			std::string sid = message.at("sid");
			std::string username = message.at("username");
			createNewConnection(sid, username);
		}
	}

	else if (common::stringCompare(stage, "lobby")) {
		if (common::stringCompare(task, "create_game")) {
			createGame(message);
		}
	}

	else if (common::stringCompare(stage, "pregame") ||
		common::stringCompare(stage, "draw") ||
		common::stringCompare(stage, "game")) {
		m_current_game->handleMessage(message);
	}

	else {
		common::print("ERROR: Provided message has no routing setup");
		common::print(message.dump());
	}
}

void Session::createNewConnection(std::string sid, std::string username) {
	// Players SID already exists in this session
	if (sidInSession(sid)) {
		common::print("Player is already in session");
		return;
	}

	std::string player_id = generatePlayerID();
	Connection* new_conn = new Connection(player_id, username, sid);

	m_connection_list.push_back(new_conn);
	m_player_ids.insert(player_id);

	m_player_count = (int)m_connection_list.size();
	if (m_player_count == 1) { setHostPlayer(player_id); }


	S2CMessages::sendJoinSessionAck(new_conn, m_id);
	S2CMessages::sendSharePlayerAttributes(m_connection_list);
	S2CMessages::sendBroadcastHostPlayer(m_connection_list, m_host_connection->getID());
	// S2CMessages::sendBroadcastNewPlayer(m_connection_list, player_id, username);
}

void Session::removeSID(std::string sid) {
	if (!sidInSession(sid)) { return; }

	Connection* target_conn = getConnectionFromSID(sid);
	if (target_conn != nullptr) { target_conn->disconnectSID(); }

	m_player_sids.erase(sid);
}

std::string Session::generatePlayerID() {
	std::string random_string = "";
	do {
		const std::string CHARACTERS = "bcdfghjklmpqrtv";

		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

		random_string = "p_";
		for (int i = 0; i < PLAYER_ID_LENGTH - 2; ++i) {
			random_string += CHARACTERS[distribution(generator)];
		}
	} while (m_player_ids.find(random_string) != m_player_ids.end());
	common::print("---PlayerID created");
	return random_string;
}

std::string Session::getID() const {
	return m_id;
}

bool Session::sidInSession(std::string sid) const {
	return m_player_sids.find(sid) != m_player_sids.end();
}

void Session::setHostPlayer(std::string player_id) {
	for (auto const& con : m_connection_list) {
		if (common::stringCompare(con->getID(), player_id)) {
			m_host_connection = con;
			return;
		}
	}
}

Connection* Session::getConnectionFromSID(std::string sid) const {
	for (auto const& curr_conn : m_connection_list) {
		if (common::stringCompare(curr_conn->getSID(), sid)) {
			return curr_conn;
		}
	}
	return nullptr;
}

void Session::createGame(JSON message) {
	if (m_current_game != nullptr) {
		common::print("A game already exists for Session " + m_id);
		return;
	}

	std::string game = message.at("game");

	if (common::stringCompare(game, "shengji")) {
		if (checkLobbyReadyForGame(game)) {
			m_current_game = new ShengJi();
			S2CMessages::sendStartPregame(m_connection_list, "shengji");
		}
	}

	else {
		common::print("ERROR: Session.createGame does not recognize the provided game name:" + game);
		common::print("--Game creation message:" + message.dump());
		common::print("");
	}
}

bool Session::checkLobbyReadyForGame(std::string game) const {
	if (common::stringCompare(game, "shengji")) {
		if (m_connection_list.size() % 2 != 0) {  //even number of players
			std::string message = "There must be an even number of players in the lobby to transition to pregame.";
			S2CMessages::sendLobbyNotReady(m_host_connection, message);
			return false;
		}
	}

	return true;
}