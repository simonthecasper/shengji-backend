#include "Deck.h"
#include <time.h>
#include <iostream>

Deck::Deck() {
	m_all_cards = {};
	m_new_deck = {};
	m_draw_deck = {};
	m_discard_deck = {};
}

Deck::Deck(int number_of_decks) {
	m_all_cards = {};
	m_new_deck = {};
	m_draw_deck = {};
	m_discard_deck = {};

	createDeck(number_of_decks, {});
}

Deck::Deck(int number_of_decks, std::vector<std::string> ranks_to_omit) {
	m_all_cards = {};
	m_new_deck = {};
	m_draw_deck = {};
	m_discard_deck = {};

	createDeck(number_of_decks, ranks_to_omit);
}


void Deck::createDeck(int number_of_decks, std::vector<std::string> ranks_to_omit) {
	const std::list<std::string> all_values = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };
	const std::list<std::string> all_jokers = { "Small Joker", "Big Joker" };

	int id = 1;
	int rank_max = 0;
	for (std::string suit : m_suits) {
		int rank = 2;

		//Create 2-A of all suits
		for (std::string value : all_values) {
			//If value will not be omitted
			if (std::find(ranks_to_omit.begin(), ranks_to_omit.end(), value) == ranks_to_omit.end()) {
				for (int i = 0; i < number_of_decks; i++) {
					Card* newCard = new Card(suit, value, rank, id);
					m_new_deck.push_back(newCard);
					m_all_cards.push_back(newCard);
					id++;
					rank_max = rank > rank_max ? rank : rank_max;
				}
				rank++;
			}
		}
	}


	//Create Jokers
	for (std::string joker : all_jokers) {
		int rank = rank_max + 1;
		//If value will not be omitted
		if (std::find(ranks_to_omit.begin(), ranks_to_omit.end(), joker) == ranks_to_omit.end()) {
			for (int i = 0; i < number_of_decks; i++) {
				Card* newCard = new Card("Joker", "Small Joker", rank, id);
				m_new_deck.push_back(newCard);
				m_all_cards.push_back(newCard);
				id++;
			}
			rank++;
		}
	}
}


void Deck::shuffleDeck() {
	std::list<Card*>::iterator m_new_deck_top = m_new_deck.begin();
	std::list<Card*>::iterator m_new_deck_search = m_new_deck.begin();

	srand(time(NULL));

	while (m_new_deck.size() > 0) {
		m_new_deck_search = m_new_deck.begin();
		int card_pull = rand() % m_new_deck.size();

		advance(m_new_deck_search, card_pull);
		m_draw_deck.push_back(*m_new_deck_search);
		m_new_deck.erase(m_new_deck_search);
	}

	m_draw_deck_top = m_draw_deck.begin();
}


Card* Deck::drawCard() {
	m_draw_deck_top = m_draw_deck.begin();
	Card* drawn_card = (*m_draw_deck_top);
	m_discard_deck.push_back(drawn_card);
	m_draw_deck.erase(m_draw_deck_top);
	return drawn_card;
}


Card* Deck::getCard(std::string suit, std::string value, int id) {
	std::list<Card*>::iterator  itr = m_all_cards.begin();
	int i = 0;
	while (i != id) {
		itr++;
		i++;
	}
	Card* card = (*itr);
	if (card->getSuit() == suit && card->getValue() == value && card->getID() == id) {
		return card;
	}
	else {
		return nullptr;
	}
}


std::string Deck::printRankOrder() {
	std::list<Card*>::iterator draw_search = m_all_cards.begin();
	int pos = 0;

	std::string ret = "";

	for (int i = 2; i < 18; i++) {
		ret += std::to_string(i) + ": ";
		draw_search = m_all_cards.begin();
		pos = 0;
		while (pos < m_all_cards.size()) {
			if ((**draw_search).getRank() == i) {			//*draw_search is a Card*.
				ret += (**draw_search).getValue() + " ";	//**draw_search is a Card.
			}
			draw_search++;
			pos++;
		}
		ret += "\n";
	}

	std::cout << ret << std::endl;
	return ret;
}


int Deck::getDrawDeckSize() const {
	return m_draw_deck.size();
}


void Deck::setStrongSuitAndValue(std::string strong_suit, std::string value) {
	std::unordered_map<std::string, int> value_to_rank;
	value_to_rank["2"] = 2;
	value_to_rank["3"] = 3;
	value_to_rank["4"] = 4;
	value_to_rank["5"] = 5;
	value_to_rank["6"] = 6;
	value_to_rank["7"] = 7;
	value_to_rank["8"] = 8;
	value_to_rank["9"] = 9;
	value_to_rank["10"] = 10;
	value_to_rank["Jack"] = 11;
	value_to_rank["Queen"] = 12;
	value_to_rank["King"] = 13;
	value_to_rank["Ace"] = 14;

	std::list<Card*>::iterator all_card_iter = m_all_cards.begin();
	int pos = 0;

	while (pos < m_all_cards.size()) {
		(**all_card_iter).setStrongSuitAndRank(strong_suit, value_to_rank[value]);
		all_card_iter++;
		pos++;
	}
}


// TODO
int Deck::deleteDeck() {
	return 0;
}