#include "Card.h"


Card::Card() {
    m_suit = "Unset";
    m_value = "Unset";
    m_rank = -1;

    m_strong = false;
    m_points = -1;

    m_id = -1;
}


Card::Card(std::string suit_con, int rank_con, int id) {
    m_suit = suit_con;
    m_rank = rank_con;
    m_id = id;

    m_strong = false;


    //Set name
    if (m_rank < 11)
        m_value = std::to_string(m_rank);
    else if (m_rank == 11)
        m_value = "Jack";
    else if (m_rank == 12)
        m_value = "Queen";
    else if (m_rank == 13)
        m_value = "King";
    else if (m_rank == 14)
        m_value = "Ace";
    else if (m_rank == 16)
        m_value = "Small Joker";
    else if (m_rank == 17)
        m_value = "Big Joker";
    else
        m_value = "Error";

    //Set points
    if (m_value == "5") {
        m_points = 5;
    }
    else if (m_value == "10" || m_value == "King") {
        m_points = 10;
    }
    else {
        m_points = 0;
    }

}

std::string Card::getSuit() const {
    return m_suit;
}

std::string Card::getValue() const {
    return m_value;
}

int Card::getRank() const {
    return m_rank;
}

bool Card::isStrong() const {
    return m_strong;
}

int Card::getPoints() const {
    return m_points;
}

int Card::getID() const {
    return m_id;
}

void Card::setStrongSuitAndRank(std::string strong_suit, int strong_rank) {
    m_strong = (m_suit == strong_suit) || (m_rank == strong_rank);
    //m_strong_suit = strong_suit;
    //m_strong_rank = strong_rank;

    if (m_rank == strong_rank) {
        if (m_suit == strong_suit)
            m_rank = 15;
        else
            m_rank = 14;
    }
    else if (m_rank > strong_rank && m_rank < 16)
        m_rank--;
    else if (m_rank >= 16) //is joker
        m_strong = true;

    //m_suit_rank["Joker"] = 4;
    if (strong_suit == "Spades") {
        m_suit_rank["Hearts"] = 3;
        m_suit_rank["Clubs"] = 2;
        m_suit_rank["Diamonds"] = 1;
    }
    else if (strong_suit == "Hearts") {
        m_suit_rank["Spades"] = 3;
        m_suit_rank["Diamonds"] = 2;
        m_suit_rank["Clubs"] = 1;
    }
    else if (strong_suit == "Clubs") {
        m_suit_rank["Hearts"] = 3;
        m_suit_rank["Spades"] = 2;
        m_suit_rank["Diamonds"] = 1;
    }
    else if (strong_suit == "Diamonds") {
        m_suit_rank["Spades"] = 3;
        m_suit_rank["Hearts"] = 2;
        m_suit_rank["Clubs"] = 1;
    }
}


bool Card::isIdentical(Card other) const {
    return m_id == other.getID();
}


bool Card::isSameRank(Card other) const {
    if (m_strong && (m_strong == other.isStrong()))
        return m_rank == other.getRank();
    else
        return (m_suit == other.getSuit()) && (m_rank == other.getRank());
}


//Returns positive if this Card is less than the other card
//Returns 0 if these cards are equivalent or if the comparison is void
//Returns negative if this Card is greater than the other card
int Card::isLessThan(const Card& other) const {
    if (m_value == "Unset")  //unset should always be less than other card, even another unset
        return 1;
    else if (other.getValue() == "Unset")
        return -1;

    if (isIdentical(other)) {
        return triStateBool(m_id, other.getID());
    }

    if (m_strong == other.isStrong()) {  //both strong or not strong
        if (m_strong)  //both strong
            return triStateBool(m_rank, other.getRank());
        else {  //both not strong
            if (m_suit == other.getSuit()) {  //same suit
                return triStateBool(m_rank, other.getRank());
            }
            else {  //different weak suits
                return triStateBool(m_suit_rank.at(m_suit), m_suit_rank.at(other.getSuit()));
            }
        }
    }
    else if (m_strong != other.isStrong()) { //one strong, one not strong
        if (other.isStrong())
            return 1;
        else
            return -1;
    }

    return false;
}


int Card::isGreaterThan(const Card& other) const {
    return -1 * isLessThan(other);
}


int Card::triStateBool(int a, int b) const {
    return b - a;
}


bool Card::operator<(const Card& other) const {
    int lessThan = isLessThan(other);

    if (lessThan == 0) {
        return m_id < other.getID();
    }
    else {
        return lessThan > 0;
    }
}


bool Card::operator>(Card other) const {
    if (m_value == "Unset")
        return false;
    else if (other.getValue() == "Unset")
        return true;

    if (m_strong == other.isStrong()) {  //both strong or not strong
        if (m_strong) { //both strong
            return m_rank > other.getRank();
        }
        else { //both not strong
            if (m_suit == other.getSuit()) {  //same suit
                return m_rank > other.getRank();
            }
            else {  //different weak suits. should be false for both
                return m_id < other.getID();
            }
        }
    }
    else if (m_strong != other.isStrong()) { //one strong, one not strong
        return m_strong;
    }

    return false;
}


//bool Card::operator==(Card other) const {
//    return isSameRank(other);
//}


std::string Card::toString() const {
    std::string ret = "";

    if (m_rank >= 16)
        ret += m_value;
    else
        ret += m_value + " of " + m_suit;

    if (m_strong)
        ret += " (strong)";

    return ret;
}

std::string Card::toJSONString() const {

}