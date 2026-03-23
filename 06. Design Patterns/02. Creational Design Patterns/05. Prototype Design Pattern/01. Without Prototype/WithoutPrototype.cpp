#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Simple NPC class — no Prototype
class NPC {
public:
    string name;
    int health;
    int attack;
    int defense;

    // “Heavy” constructor: every field must be provided
    NPC(const string& name, int health, int attack, int defense) {

        // call database
        // complex calc
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;

        cout << "Creating NPC '" << name << "' [HP:" << health << ", ATK:" 
             << attack << ", DEF:" << defense << "]\n";
    }

    void describe() {
        cout << "  NPC: " << name << " | HP=" << health << " ATK=" << attack
             << " DEF=" << defense << "\n";
    }
};

int main() {
    // Base Alien
    NPC* alien = new NPC("Alien", 30, 5, 2);
    alien->describe();

    // Powerful Alien — must re-pass all stats, easy to make mistakes
    NPC* alien2 = new NPC("Powerful Alien", 30, 5, 5);
    alien2->describe();

    // If you want 100 aliens, you'd repeat this 100 times…

    // cleanup
    delete alien;
    delete alien2;
    return 0;
}