#include "AcceptedSocket.h"


AcceptedSocket::AcceptedSocket() {

}

AcceptedSocket::AcceptedSocket(int socketFD, sockaddr_in address, int error, bool accepted_successful) {
	m_socketFD = socketFD;
	m_address = address;
	m_error = error;
	m_accepted_successful = accepted_successful;
}

int AcceptedSocket::getSocketFD() const {
	return m_socketFD;
}

std::string AcceptedSocket::toString() const {
	char new_con[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(m_address.sin_addr), new_con, sizeof(new_con));
	return new_con;
}