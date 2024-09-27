
#include "SocketServer.h"
#include "Deck.h"

#include <iostream>
#include <set>

void deckTesting();
int serverTesting();

int main() {
	serverTesting();
	// deckTesting();

	return 0;
}


int serverTesting() {
	SocketServer s;
	s.runServerAsAppServer();
	return 0;
}