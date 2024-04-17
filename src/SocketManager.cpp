#include "SocketManager.h"


SocketManager::SocketManager(int session_id) {
	m_session_id = session_id;
}

int SocketManager::addPlayerSocket(int id, SOCKET socket_fd) {
	if (m_player_sockets.find(id) != m_player_sockets.end()) {
		std::cout << "Player ID: " << id << " is already present in this SocketManager.\n";
		return -1;
	}

	m_player_sockets[id] = socket_fd;
	return 0;
}

SOCKET SocketManager::getPlayerSocket(int id) {
	if (m_player_sockets.find(id) == m_player_sockets.end()) {
		std::cout << "Player ID: " << id << " is not found in this SocketManager.\n";
		return -1;
	}

	return m_player_sockets.at(id);
}