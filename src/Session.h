#pragma once

//#include "common.h"
#include "Deck.h"
#include "Player.h"
#include "Chat.h"

class SocketServer;

enum MatchState {
	DRAWING,
	AWAITING_TRICK_START,
	AWAITING_TRICK_FOLLOW
};

class Session
{
private:
	//SocketServer*			m_server;

	std::string				m_id;
	Chat*					m_chatlog;

	unordered_set<int>		m_player_ids;
	unordered_set<SOCKET>	m_player_sockets;
	list<Player*>			m_player_list;
	unordered_map<int, SOCKET>
							m_id_to_socket;

	//administrative fields
	//Deck*					m_deck;
	//unordered_map<int,Player*>
	//						m_teams;
	//unordered_map<int, int>
	//						m_match_score;


	//gametime fields
	//MatchState			m_state;
	int					m_player_count;
	//int					m_game_score;
	//
	//Player*				m_trick_starter;
	//Player*				m_current_player;
	//Player*				m_next_player;

	//int					m_match_state;

public:

	Session(std::string id);

	void startGame();

	void addToChat(JSON message_json);

	// Accepts the SocketFD for a new player to add to the session and
	//   return the ID this player gets assigned.
	int addPlayer(SOCKET m_player_fd);

	int generateID();

	int handleJSON(JSON input);

	std::string getID();

	void sendToOtherPlayers(int source_player, JSON message);
};