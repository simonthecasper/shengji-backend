#pragma once

//#include "common.h"
#include "Deck.h"
#include "Player.h"
#include "Chat.h"
#include "Game.h"

#define PLAYER_ID_LENGTH	10


class Session {
private:
	std::string		m_id;
	Chat* m_chatlog;
	int				m_player_count;

	Player* m_host;
	Game			m_current_game;

	std::list<Player*>						m_player_list;
	std::unordered_set<std::string>			m_player_ids;
	std::unordered_set<std::string>			m_player_sids;

	std::unordered_map<std::string, Player*> 		m_id_to_player;
	std::unordered_map<std::string, std::string>		m_sid_to_id;
	std::unordered_map<std::string, std::string>		m_id_to_sid;

	std::unordered_map<std::string, std::string>		m_player_teams; // <player_id, team>

public:
	Session();

	//Creates a session with the provided ID
	Session(std::string id);

	//TODO:General message handler that routes messages to more specific handlers
	void handleMessage(JSON message_json);

	//Receives a chat message and adds it to the session chat
	void addToChat(JSON message_json);

	//Accepts a JSON message and sends it to every user in this session EXCEPT for
	// 	the user with the provided SID
	void sendToOtherPlayersSID(std::string source_sid, JSON message);

	//Adds a new player into this session with the provided SID.
	//	Returns the player_id of this newly added player
	std::string addPlayerSID(std::string sid);

	//Removes the player with the provided SID from the session
	void removePlayerSID(std::string sid);

	//Generates a new, unused ID string for a player
	std::string generatePlayerID();

	//Returns the session ID of this session
	std::string getID();

	//TODO: Adds the sender of the given message to the team they specify
	//	Updates m_team_assignments accordingly and sets the team field in the player object
	void addPlayerToTeam(JSON message);

	//TODO: Creates a Game object for when the game starts
	void createGame();

};