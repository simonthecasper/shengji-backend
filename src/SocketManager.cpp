#include "SocketManager.h"


SocketManager::SocketManager(int session_id) {
	m_session_id = session_id;
}

int SocketManager::addPlayerSocket(int id, int socket_fd) {
	if (m_playerid_to_socket.find(id) != m_playerid_to_socket.end()) {
		std::cout << "Player ID: " << id << " is already present in this SocketManager.\n";
		return -1;
	}

	m_playerid_to_socket[id] = socket_fd;
	return 0;
}

int SocketManager::getPlayerSocket(int id) {
	if (m_playerid_to_socket.find(id) == m_playerid_to_socket.end()) {
		std::cout << "Player ID: " << id << " is not found in this SocketManager.\n";
		return -1;
	}

	return m_playerid_to_socket.at(id);
}