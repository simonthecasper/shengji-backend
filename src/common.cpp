#pragma once

#include "common.h"


int common::sendThroughSocket(SOCKET destination, std::string message_str) {
	const char* message_char = message_str.c_str();
	int send_result = send(destination, message_char, (int)strlen(message_char), 0);

	if (send_result == SOCKET_ERROR) {
		std::cout << "Error sending message to socket " << destination << "." << std::endl;
		return -1;
	}

	return 0;
}

void common::check(int input, std::string instance) {
	if (input == -1) {
		std::cout << "Error at " << instance << std::endl;
		exit(2);
	}
}