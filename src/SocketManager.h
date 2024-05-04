#pragma once
#include <unordered_map>
#include <sys/socket.h>
#include <iostream>

class SocketManager
{
private:
	int m_session_id;
	std::unordered_map<int, int> m_playerid_to_socket;

public:
	SocketManager(int session_id);

	int addPlayerSocket(int id, int socket_fd);

	int getPlayerSocket(int id);


};

