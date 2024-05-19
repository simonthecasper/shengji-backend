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
	ws_server* m_wss;

	std::string						m_id;
	Chat* m_chatlog;


	set<Player*>					m_player_list;

	unordered_set<int>				m_player_ids;

	////////////////PLAIN SOCKETS/////////////////
	unordered_set<int>					m_player_sockets;
	unordered_map<int, int>				m_id_to_socket;
	unordered_map<int, int>				m_socket_to_id;
	unordered_map<int, Player*> 		m_id_to_player;


	////////////////Web Sockets///////////////////
	// unordered_set<ws_conn_hdl>			m_player_handles;
	unordered_map<int, ws_conn_hdl>		m_id_to_handle;
	// unordered_map<ws_conn_hdl, int>		m_handle_to_id;



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

	//Player*				m_trick_starter;
	//Player*				m_current_player;
	//Player*				m_next_player;

	//int					m_match_state;

public:

	Session(std::string id);

	void addToChat(JSON message_json);



	/*-------------------------------------------*/
	/*                Websocket                  */
	/*-------------------------------------------*/

	// Accepts the SocketFD for a new player to add to the session and
	//   return the ID this player gets assigned.
	int addPlayer_ws(ws_conn_hdl player_hdl);

	void sendToOtherPlayers_ws(int source_player, JSON message);

	void removePlayer_ws(ws_conn_hdl player_hdl);

	void addToChat_ws(msg_hdl_pair);

	/*-------------------------------------------*/
	/*              Python Client                */
	/*-------------------------------------------*/

	// Accepts the SocketFD for a new player to add to the session and
	//   return the ID this player gets assigned.
	int addPlayer(int player_fd);

	void sendToOtherPlayers(int source_player, JSON message);

	void removePlayer(int socket);



	int generateID();

	int handleJSON(JSON input);

	std::string getID();


};