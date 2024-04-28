#pragma once


#include "Session.h"

#define	ID_MAX		10000

class SocketServer;


class SessionManager
{
private:
	//SocketServer*		m_server;
	std::unordered_map<std::string, Session*>
						m_id_to_session;

public:
	SessionManager();

	Session* createNewSession();

	int addPlayerToSession(std::string session_id, SOCKET player);

	void receiveJSON(JSON message);

private:
	bool ifSessionIDExists(std::string id);

	std::string generateSessionID();	
};

