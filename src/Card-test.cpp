#include <string>

using namespace std;

class Cardtest
{
    private:
        string suit;
        int value;
        bool strong;


    public:
        Cardtest(string suit_con, int value_con)
        {
            suit = suit_con;
            value = value_con;
            strong = false;
        }

        string getSuit() {
            return suit;
        }

        int getValue() {
            return value;
        }

        bool isStrong() {
            return strong;
        }

        void setStrongSuit(string strong_suit) {
            strong = suit == strong_suit;
        }
        
        // Assumes cards are the same suit
        bool operator <(Cardtest other) {
            if (strong == other.isStrong()) {
                return value < other.getValue();
            } else {
                return !strong;
            }
        }

        bool operator >(Cardtest other) {
            if (strong == other.isStrong()) {
                return value > other.getValue();
            }
            else {
                return strong;
            }
        }

    
};