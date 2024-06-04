#pragma once

//#include "common.h"
#include "Deck.h"
#include "Player.h"
#include "Chat.h"


class Session
{
private:
	std::string						m_id;
	Chat* m_chatlog;

	set<Player*>					m_player_list;
	unordered_set<int>				m_player_ids;

	////////////////PLAIN SOCKETS/////////////////
	unordered_set<int>					m_player_sockets;
	unordered_map<int, int>				m_id_to_socket;
	unordered_map<int, int>				m_socket_to_id;
	unordered_map<int, Player*> 		m_id_to_player;

	/////////SOCKETIO APPLICATION SERVER///////////
	unordered_map<std::string, int>				m_sid_to_id;
	unordered_map<int, std::string>				m_id_to_sid;
	unordered_set<std::string>					m_player_sids;


	int					m_player_count;

public:

	Session(std::string id);

	void addToChat(JSON message_json);

	/*-------------------------------------------*/
	/*              Python Client                */
	/*-------------------------------------------*/

	void sendToOtherPlayers(int source_player, JSON message);



	/*-------------------------------------------*/
	/*           SocketIO App Server             */
	/*-------------------------------------------*/

	int addPlayerSID(std::string sid);

	void sendToOtherPlayersSID(std::string source_sid, JSON message);

	void removePlayerSID(std::string sid);

	void addToChat_SID(JSON message, std::string sid);



	/*-------------------------------------------*/
	/*                 Common                    */
	/*-------------------------------------------*/

	int generateID();

	int handleJSON(JSON input);

	std::string getID();


};