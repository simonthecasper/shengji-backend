#pragma once

//#include "common.h"
#include "Deck.h"
#include "Player.h"
#include "Chat.h"
#include "Game.h"
#include "S2CMessages.h"

#define PLAYER_ID_LENGTH	10


class Session {
private:
	std::string		m_id;
	Chat* m_chatlog;
	int				m_player_count;

	Player* m_host_player;
	Game* m_current_game;

	std::list<Player*>						m_player_list;
	std::unordered_set<std::string>			m_player_ids;
	std::unordered_set<std::string>			m_player_sids;

	std::unordered_map<std::string, Player*> 		m_id_to_player;
	std::unordered_map<std::string, std::string>		m_sid_to_id;
	std::unordered_map<std::string, std::string>		m_id_to_sid;

	std::unordered_map<std::string, std::string>		m_player_teams; // <player_id, team>
	std::mutex											m_set_team_mutex;

public:
	Session();

	//Creates a session with the provided ID
	Session(std::string id);

	//TODO:General message handler that routes messages to more specific handlers
	void handleMessage(JSON message_json);

	//Receives a chat message and adds it to the session chat
	void addToChat(JSON message);

	//Accepts a JSON message and sends it to every user in this session EXCEPT for
	// 	the user with the provided SID
	void sendToOtherPlayersSID(std::string source_sid, JSON message);

	//Adds a new player into this session with the provided SID.
	//  Send a confirmation message to the SID if successful
	void addPlayer(std::string sid, std::string username);

	//Removes the player with the provided SID from the session
	void removePlayerSID(std::string sid);

	//Generates a new, unused ID string for a player
	std::string generatePlayerID();

	//Returns the session ID of this session
	std::string getID();

	//Adds the player with the provided player_id to the given team
	void addPlayerToTeam(std::string player_id, std::string team);

	//Returns if a user with the given SID is already in this session
	bool sidInSession(std::string sid);

	//Sets the host player to the player with the provided player_id
	void setHostPlayer(std::string player_id);

	//TODO: Checks if game can be started and creates a Game object if valid
	void checkAndCreateGame();

	//Returns if the lobby is ready to start a game
	bool checkIfLobbyReady();

};