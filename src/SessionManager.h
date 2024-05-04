#pragma once


#include "Session.h"


#define ID_LENGTH	4
#define	ID_MAX		10000


class SocketServer;


class SessionManager
{
private:
	//SocketServer*		m_server;
	std::unordered_map<std::string, Session*>
		m_id_to_session;


	std::unordered_map<int, std::string>
		m_socket_to_sessionid;

public:
	SessionManager();

	void receiveJSON(JSON message);

	void removeSocket(int socket);

private:
	Session* createNewSession();

	int addPlayerToSession(std::string session_id, int player);

	bool ifSessionIDExists(std::string id);

	std::string generateSessionID();

	void linkSocketToSessionID(int socket, std::string id);
};

