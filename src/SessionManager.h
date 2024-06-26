#pragma once

#include "Session.h"

#define ID_LENGTH	6


class SessionManager
{
private:
	std::unordered_map<std::string, Session*>
		m_id_to_session;

	std::unordered_map<int, std::string>
		m_socket_to_sessionid;

	std::unordered_map<std::string, std::string>
		m_sid_to_sessionid;

public:
	void handleMessage(JSON message);

	void removeSID(std::string sid);

private:
	Session* createNewSession();

	std::string addPlayerToSessionSID(std::string session_id, std::string sid);

	void linkSIDToSessionID(std::string sid, std::string id);

	bool ifSessionIDExists(std::string id);

	std::string generateSessionID();
};
