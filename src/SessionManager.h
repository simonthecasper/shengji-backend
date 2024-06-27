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

	void handleMessage(JSON message);

private:
	void removeSID(std::string sid);

	Session* createNewSession();

	void linkSIDToSessionID(std::string sid, std::string id);

	bool doesSessionIDExist(std::string id);

	std::string generateSessionID();
};
