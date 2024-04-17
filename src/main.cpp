#include <iostream>
#include "WinSock2.h"
#include <list>
#include "Play.h"
#include "Deck.h"
#include "SocketServer.h"
#include "json.hpp"

using json = nlohmann::json;


void deckTesting();
int serverTesting();

int main() {
	
	serverTesting();
	//deckTesting();
	
	
 	return 0;
}


int serverTesting() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return 1;
	}
	else
		printf("The Winsock 2.2 dll was found okay\n");


	/* The Winsock DLL is acceptable. Proceed to use it. */

	SocketServer s;

	/* then call WSACleanup when done using the Winsock dll */

	while (true) {

	}

	WSACleanup();
}


void deckTesting() {
	int deck_count = 2;
	int card_omit = 0;
	Deck deck(deck_count, card_omit);

	deck.shuffleDeck();
	deck.setStrongSuitAndValue("Spades", "10");
	//Card* card = deck.drawCard();
	//std::cout << deck.printRankOrder() << std::endl;

	set<Card> cardset;

	int draw_deck_size = deck.getDrawDeckSize();
	for (int i = 0; i < draw_deck_size; i++) {
		Card* drawnCard = deck.drawCard();
		std::cout << to_string(i) << ": " << (*drawnCard).toString() << "\n";
		cardset.insert(*drawnCard);
	}

	std::cout << "\n" << std::endl;


	//Card* get_card = deck.getCard("Spades", "2", 0);
	//std::cout << get_card->toString() << std::endl;

	//for (Card card : cardset) {
	//	std::cout << card.toString() << "\n";
	//}
	//std::cout << to_string(cardset.size()) << std::endl;

	deck.printRankOrder();
}