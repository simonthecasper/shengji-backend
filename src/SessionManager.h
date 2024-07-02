#pragma once

#include "Session.h"

#define ID_LENGTH	6


class SessionManager
{
private:
	std::unordered_map<std::string, Session*>
		m_id_to_session;

	std::unordered_map<std::string, std::string>
		m_sid_to_sessionid;

public:
	SessionManager();

	// General message handler that routes messages to more specific handlers.
	void handleMessage(JSON message);

private:

	// Creates a new session and returns the pointer to the created Session
	Session* createNewSession();

	// Associates a socketio SID to the session the user is joined into
	void linkSIDToSessionID(std::string sid, std::string id);

	// Returns whether a Session with the provided ID already exists
	bool doesSessionIDExist(std::string id);

	// Removes a socketio SID from the session the user is joined into.
	void removeSID(std::string sid);

	// Generates a new ID for a Session
	std::string generateSessionID();
};
