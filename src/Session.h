#pragma once

#include "Connection.h"
#include "Deck.h"
#include "Chat.h"
#include "ShengJi.h"
#include "S2CMessages.h"

#define PLAYER_ID_LENGTH	10

enum SessionState { open, closed };

class Session {
private:
	std::string		m_id;
	SessionState	m_state;
	std::mutex		m_session_mutex;

	Chat* m_chatlog;

	ShengJi* m_current_game;

	Connection* m_host_connection;
	std::list<Connection*>					m_connection_list;
	std::unordered_set<std::string>			m_player_ids;
	std::unordered_set<std::string>			m_player_sids;
	int				m_player_count;


public:
	Session();

	// Creates a session with the provided ID
	Session(std::string id);

	// General message handler that routes messages to more specific handlers
	void handleMessage(JSON message_json);

	// Returns the session ID of this session
	std::string getID() const;

	// Removes the player with the provided SID from the session
	void removeSID(std::string sid);

private:
	//Adds a new player into this session with the provided SID.
	//  Sends a confirmation message to the SID if successful
	void createNewConnection(std::string sid, std::string username);

	//Generates a new, unused ID string for a player
	std::string generatePlayerID();

	//Returns if a user with the given SID is already in this session
	bool sidInSession(std::string sid) const;

	//Sets the host player to the player with the provided player_id
	void setHostPlayer(std::string player_id);

	// Returns a pointer to the Connection with the provided SID attached to it
	Connection* getConnectionFromSID(std::string sid) const;

	// Creates an instance of the desired game for this session
	void createGame(JSON message);

	// Checks if the lobby is ready to transition into the pregame stage for the requested game
	// Returns true if the lobby is ready
	// Returns false and sends a message to the host connection if the lobby is not ready
	bool checkLobbyReadyForGame(std::string game) const;
};