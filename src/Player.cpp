#include "Player.h"
#include "Play.h"


Player::Player() {

}

Player::Player(std::string id) {
	m_id = id;
}


Player::Player(std::string id, std::string username) {
	m_id = id;
	m_username = username;
	m_team = "Unset";
}

std::string Player::getUsername() const {
	return m_username;
}

std::string Player::getID() const {
	return m_id;
}

void Player::addCardToHand(Card* card) {
	m_hand.insert(card);
}


bool Player::ifHasCard(std::string suit, int rank, int id) {
	std::set<Card*>::iterator itr;
	itr = m_hand.begin();

	while (itr != m_hand.end()) {
		Card c = **itr;
		if (c.getSuit() == suit && c.getRank() == rank && c.getID() == id) {
			return true;
		}
		itr++;
	}
	return false;
}

bool Player::ifHasCard(Card* card) {
	return m_hand.find(card) != m_hand.end();
}

std::set<Card*> Player::getHand() const {
	return m_hand;
}

int Player::getHandSize() const {
	return m_hand.size();
}

void Player::setTeam(std::string team) {
	m_team = team;
}

std::string Player::getTeam() {
	return m_team;
}