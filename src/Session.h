#pragma once

//#include "common.h"
#include "Deck.h"
#include "Player.h"
#include "Chat.h"

#define PLAYER_ID_LENGTH	10


class Session {
private:
	std::string		m_id;
	Chat* m_chatlog;
	int				m_player_count;

	set<Player*>						m_player_list;
	unordered_set<std::string>			m_player_ids;
	unordered_set<std::string>			m_player_sids;

	unordered_map<std::string, Player*> 		m_id_to_player;
	unordered_map<std::string, std::string>		m_sid_to_id;
	unordered_map<std::string, std::string>		m_id_to_sid;


public:
	Session(std::string id);

	void addToChat(JSON message_json);

	void sendToOtherPlayersSID(std::string source_sid, JSON message);

	std::string addPlayerSID(std::string sid);

	void removePlayerSID(std::string sid);

	std::string generatePlayerID();

	std::string getID();

};