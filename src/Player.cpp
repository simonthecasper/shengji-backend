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

void Player::setColor(std::string color) {
	m_color = color;
	updatePlayerAttributes();
}

std::string Player::getColor() const {
	return m_color;
}

void Player::setRevealElegibility(bool elegible) {
	m_reveal_eligible = elegible;
}

bool Player::toggleRevealEligibility() {
	m_reveal_eligible = !m_reveal_eligible;
	return m_reveal_eligible;
}

bool Player::getRevealElegibility() {
	return m_reveal_eligible;
}

void Player::setProgressCard(std::string card_value) {
	if (!common::isProgressCard(card_value)) {
		std::cout << "The provided card is not a Progress Card" << std::endl;
		return;
	}

	m_progress_card = card_value;
}

std::string Player::incrementProgressCard() {
	std::vector<std::string>::iterator itr = std::find(common::ProgressCards.begin(), common::ProgressCards.end(), m_progress_card);
	if (itr++ != common::ProgressCards.end()) {
		m_progress_card = *itr;
	}
	else {
		m_progress_card = "Finished";
	}

	return m_progress_card;
}


void Player::updatePlayerAttributes() {
	m_player_attributes = {
		{"username", m_username},
		{"color", m_color}
	};
}

JSON Player::getPlayerAttributes() const {
	return m_player_attributes;
}