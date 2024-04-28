#pragma once

#include "common.h"


using namespace std;


class Card
{
private:
    string m_suit;
    string m_value;
    int m_rank;
    bool m_strong;
    int m_points;

    //Unique number assigned to each card to differentiate cards with identical ranks and suits.
    int m_id;
    
    unordered_map<string, int> m_suit_rank;

public:
    
    int testfield;

    Card();

    Card(string suit_con, int rank_con, int id);
    
    //Returns the suit of the card as a string
    string getSuit() const;

    //Returns the value of the card
    string getValue() const;

    //Returns the rank of the card
    int getRank() const;

    //Returns whether the card is considered strong
    bool isStrong() const;

    //Returns the points the card is worth
    int getPoints() const;

    //Returns the unique ID number of the card
    int getID() const;

    //Updates the m_strong field to true if the card suit matches the provided strong suit
    void setStrongSuitAndRank(string strong_suit, int strong_rank);

    //Returns true if Cards are identical.
    bool isIdentical(Card other) const;

    //Returns true if Cards have the same rank (even if not identical).
    bool isSameRank(Card other) const;


    //
    int isLessThan(const Card& other) const;


    int isGreaterThan(const Card& other) const;


    int triStateBool(int a, int b) const;

    //Returns true if left Card has a value smaller than right.
    //Returns false if left Card has a value larger than right,
    //        or if two cards of different weak suits are being compared.
    bool operator<(const Card &other) const;
    
    //Returns true if left Card has a value larger than right.
    //Returns false if left Card has a value smaller than right,
    //        or if two cards of different weak suits are being compared.
    bool operator>(Card other) const;

    //Returns true if the two Cards are identical in suit and rank.
    //bool operator==(Card other) const;

    string toString() const;
};
