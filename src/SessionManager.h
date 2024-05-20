#pragma once


#include "Session.h"


#define ID_LENGTH	4
#define	ID_MAX		10000


class SocketServer;


class SessionManager
{
private:
	ws_server* m_wss;


	std::unordered_map<std::string, Session*>
		m_id_to_session;


	std::unordered_map<int, std::string>
		m_socket_to_sessionid;


	std::unordered_map<std::string, std::string>
		m_sid_to_sessionid;

public:

	void receiveJSON(JSON message);

	void receiveJSON_AppServer(JSON message);

	void receiveWork_ws(std::pair<JSON, ws_conn_hdl> work);

	void removeSocket(int socket);

	void removeSID(std::string sid);

private:
	Session* createNewSession();

	int addPlayerToSession(std::string session_id, int player_fd);

	int addPlayerToSession_ws(std::string session_id, ws_conn_hdl player_hdl);

	int addPlayerToSessionSID(std::string session_id, std::string sid);

	void linkSocketToSessionID(int socket, std::string id);

	void linkSIDToSessionID(std::string sid, std::string id);



	bool ifSessionIDExists(std::string id);

	std::string generateSessionID();


};

