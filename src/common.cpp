#include "common.h"


int common::sendThroughSocket(int destination, JSON message_json) {
	std::string message_str = message_json.dump();
	return sendThroughSocket(destination, message_str);
}

int common::sendThroughSocket(int destination, std::string message_str) {
	m_socket_send_mutex.lock();

	std::cout << "Sending through socket " << destination << ": " << message_str << std::endl;

	const char* message_char = message_str.c_str();
	int send_result = send(destination, message_char, (int)strlen(message_char), 0);

	if (send_result == -1) {
		std::cout << "Error sending message to socket " << destination << "." << std::endl;
		m_socket_send_mutex.unlock();
		return -1;
	}

	m_socket_send_mutex.unlock();
	return 0;
}

int common::sendThroughSocketSID(JSON message_json) {
	std::string message_str = message_json.dump();
	return sendThroughSocketSID(message_str);
}

int common::sendThroughSocketSID(std::string message_str) {
	m_socket_send_mutex.lock();

	print("Sending to socketio server: " + message_str);
	std::cout << "Socket: " << m_socketio_server << std::endl;

	const char* message_char = message_str.c_str();
	int send_result = send(m_socketio_server, message_char, (int)strlen(message_char), 0);

	if (send_result == -1) {
		std::cout << "Error sending message to socket " << m_socketio_server << "." << std::endl;
		m_socket_send_mutex.unlock();
		return -1;
	}

	m_socket_send_mutex.unlock();
	return 0;
}

void common::check(int input, std::string instance) {
	if (input == -1) {
		std::cout << "Error at " << instance << std::endl;
		exit(2);
	}
}

bool common::stringCompare(std::string s1, std::string s2) {
	return strcmp(s1.c_str(), s2.c_str()) == 0;
}

void common::print(std::string s) {
	std::cout << s << std::endl;
}

void common::setSocketIOServerFD(int fd) {
	m_socketio_server = fd;
}

void common::setStartTime() {
	m_start_time = time(0);
}

double common::getTime() {
	m_time_mutex.lock();
	double diff = difftime(time(0), m_start_time);
	m_time_mutex.unlock();

	return diff;
}