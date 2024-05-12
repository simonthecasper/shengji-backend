
// #include "WebsocketServer.h"

// int main() {

// 	static WebsocketServer w;
// 	w.init();
// 	w.run();
// 	while (true) {

// 	}
// 	return 0;
// }




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main() {
	utility_server s;
	s.run();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////


// #include "SocketServer.h"
// #include "Deck.h"

// #include <iostream>
// #include <set>
// #include <openssl/sha.h>

// void deckTesting();
// int serverTesting();

// int main() {
// 	// std::string s = "GET / HTTP/1.1\r\nHost: 50.35.70.57:12345\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:125.0) Gecko/20100101 Firefox/125.0\r\nAccept: */*\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nSec-WebSocket-Version: 13\r\nOrigin: null\r\nSec-WebSocket-Extensions: permessage-deflate\r\nSec-WebSocket-Key: fEymQPSw3/f1T3P7U59mPw==\r\nDNT: 1\r\nSec-GPC: 1\r\nConnection: keep-alive, Upgrade\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nUpgrade: websocket\r\n\r\n";
// 	// int length = s.length();

// 	// std::string token = "Sec-WebSocket-Key: ";
// 	// int start = s.find(token) + token.length();

// 	// std::string key_to_end = s.substr(start, s.length() - 1);

// 	// int end = key_to_end.find("\r\n");

// 	// std::string key = s.substr(start, end);

// 	// std::cout << "s length:" << length << std::endl;
// 	// std::cout << "token start:" << start << std::endl;
// 	// std::cout << "key end:" << end << std::endl;
// 	// std::cout << "key:" << key << std::endl;

// 	serverTesting();
// 	// deckTesting();

// 	return 0;
// }


// int serverTesting() {
// 	SocketServer s;
// 	return 0;
// }


// void deckTesting() {
// 	int deck_count = 2;
// 	int card_omit = 0;
// 	Deck deck(deck_count, card_omit);

// 	deck.shuffleDeck();
// 	deck.setStrongSuitAndValue("Spades", "10");
// 	//Card* card = deck.drawCard();
// 	//std::cout << deck.printRankOrder() << std::endl;

// 	set<Card> cardset;

// 	int draw_deck_size = deck.getDrawDeckSize();
// 	for (int i = 0; i < draw_deck_size; i++) {
// 		Card* drawnCard = deck.drawCard();
// 		std::cout << to_string(i) << ": " << (*drawnCard).toString() << "\n";
// 		cardset.insert(*drawnCard);
// 	}
// 	std::cout << "\n" << std::endl;

// 	//Card* get_card = deck.getCard("Spades", "2", 0);
// 	//std::cout << get_card->toString() << std::endl;

// 	//for (Card card : cardset) {
// 	//	std::cout << card.toString() << "\n";
// 	//}
// 	//std::cout << to_string(cardset.size()) << std::endl;

// 	deck.printRankOrder();
// }