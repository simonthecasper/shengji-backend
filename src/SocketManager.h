#pragma once
#include <unordered_map>
#include <WinSock2.h>
#include <iostream>

class SocketManager
{
private:
	int m_session_id;
	std::unordered_map<int, SOCKET> m_player_sockets;

public:
	SocketManager(int session_id);

	int addPlayerSocket(int id, SOCKET socket_fd);


	SOCKET getPlayerSocket(int id);


};

