#include "Play.h"


Play::Play(Player* source) {
	m_source = source;
}

void Play::makeStartPlay(unordered_map<string, int> structure, set<Card*> cards) {
	m_structure = structure;
	m_cards = cards;
	m_valid = true;

	/*if (!throw) {

	}*/
	setHighCard();
}

void Play::makeFollowPlay(Play* starter, set<Card*> cards, bool valid) {
	m_structure = starter->getStructure();
	m_cards = cards;
	m_valid = valid;//may delete later
	//TODO: check if play is valid. May not need if it gets checked at the front


	setHighCard();
}


unordered_map<string, int> Play::getStructure() const{
	return m_structure;
}


bool Play::isValid() const{
	return m_valid;
}


Card* Play::getHighCard() const {
	return m_high_card;
}


void Play::setHighCard() {
	if (m_valid) {
		for (Card* card : m_cards) {
			if ((*m_high_card).isLessThan(*card)) {
				m_high_card = card;
			}
		}
	}
}

string Play::toString() const {
	return "asdf";
}

//Returns positive if true
//Returns negative if false
//Returns 0 if comparison is void
int Play::isLessThan(const Play& other) const {
	if (m_valid && other.isValid()) {  //both valid or invalid
		if (m_valid) {
			//both valid
			return m_high_card->isLessThan(*(other.getHighCard()));
		}
		else {
			//both not valid
			return 0;
		}
	} else {
		if (m_valid) {
			return -1;
		}
		else {
			return 1;
		}
	}
	return 0;
}


int Play::isGreaterThan(const Play& other) const {
	return -1 * isLessThan(other);
}



bool Play::operator<(const Play& other) const {
	int lessThan = isLessThan(other);

	if (lessThan == 0) {
		return m_high_card->getID() < other.getHighCard()->getID();
	}
	else {
		return lessThan > 0;
	}
}

bool Play::operator>(const Play& other) const {
	return !operator<(other);
}