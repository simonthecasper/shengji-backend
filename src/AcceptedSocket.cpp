#include "AcceptedSocket.h"


AcceptedSocket::AcceptedSocket() {

}

AcceptedSocket::AcceptedSocket(SOCKET socketFD, SOCKADDR_IN address, SOCKET error, bool accepted_successful) {
	m_socketFD = socketFD;
	m_address = address;
	m_error = error;
	m_accepted_successful = accepted_successful;
}

SOCKET AcceptedSocket::getSocketFD() const {
	return m_socketFD;
}

