#include <iostream>
using namespace std;

// Forward declaration
class ATM;

// Abstract State
class ATMState {
public:
    virtual ATMState* insertCard(ATM* atm) = 0;
    virtual ATMState* ejectCard(ATM* atm) = 0;
    virtual ATMState* enterPIN(ATM* atm, int pin) = 0;
    virtual ATMState* withdrawCash(ATM* atm, int amount) = 0;
    virtual string getStateName() = 0;
};

// Forward declare states
class NoCardState;
class HasCardState;
class AuthenticatedState;
class NoCashState;

// Context
class ATM {
private:
    ATMState* currentState;
    int cash;
    int correctPIN;

    ATMState* noCardState;
    ATMState* hasCardState;
    ATMState* authenticatedState;
    ATMState* noCashState;

public:
    ATM(int cash, int pin);

    void insertCard() {
        currentState = currentState->insertCard(this);
    }

    void ejectCard() {
        currentState = currentState->ejectCard(this);
    }

    void enterPIN(int pin) {
        currentState = currentState->enterPIN(this, pin);
    }

    void withdrawCash(int amount) {
        currentState = currentState->withdrawCash(this, amount);
    }

    void printStatus() {
        cout << "\n--- ATM STATUS ---" << endl;
        cout << "Cash Available: " << cash << endl;
        cout << "Current State: " << currentState->getStateName() << endl;
    }

    // Getters
    ATMState* getNoCardState() { return noCardState; }
    ATMState* getHasCardState() { return hasCardState; }
    ATMState* getAuthenticatedState() { return authenticatedState; }
    ATMState* getNoCashState() { return noCashState; }

    int getCash() { return cash; }
    void deductCash(int amount) { cash -= amount; }
    int getPIN() { return correctPIN; }
};

// No Card State
class NoCardState : public ATMState {
public:
    ATMState* insertCard(ATM* atm) {
        cout << "Card inserted\n";
        return atm->getHasCardState();
    }

    ATMState* ejectCard(ATM* atm) {
        cout << "No card to eject\n";
        return this;
    }

    ATMState* enterPIN(ATM* atm, int pin) {
        cout << "Insert card first\n";
        return this;
    }

    ATMState* withdrawCash(ATM* atm, int amount) {
        cout << "Insert card first\n";
        return this;
    }

    string getStateName() { return "NO_CARD"; }
};

// Has Card State
class HasCardState : public ATMState {
public:
    ATMState* insertCard(ATM* atm) {
        cout << "Card already inserted\n";
        return this;
    }

    ATMState* ejectCard(ATM* atm) {
        cout << "Card ejected\n";
        return atm->getNoCardState();
    }

    ATMState* enterPIN(ATM* atm, int pin) {
        if (pin == atm->getPIN()) {
            cout << "PIN correct\n";
            return atm->getAuthenticatedState();
        } else {
            cout << "Wrong PIN\n";
            return this;
        }
    }

    ATMState* withdrawCash(ATM* atm, int amount) {
        cout << "Enter PIN first\n";
        return this;
    }

    string getStateName() { return "HAS_CARD"; }
};

// Authenticated State
class AuthenticatedState : public ATMState {
public:
    ATMState* insertCard(ATM* atm) {
        cout << "Card already inserted\n";
        return this;
    }

    ATMState* ejectCard(ATM* atm) {
        cout << "Card ejected\n";
        return atm->getNoCardState();
    }

    ATMState* enterPIN(ATM* atm, int pin) {
        cout << "Already authenticated\n";
        return this;
    }

    ATMState* withdrawCash(ATM* atm, int amount) {
        if (atm->getCash() < amount) {
            cout << "Not enough cash in ATM\n";
            return atm->getNoCashState();
        }

        cout << "Dispensing Rs " << amount << endl;
        atm->deductCash(amount);

        if (atm->getCash() == 0) {
            return atm->getNoCashState();
        }

        return atm->getNoCardState();
    }

    string getStateName() { return "AUTHENTICATED"; }
};

// No Cash State
class NoCashState : public ATMState {
public:
    ATMState* insertCard(ATM* atm) {
        cout << "ATM out of cash\n";
        return this;
    }

    ATMState* ejectCard(ATM* atm) {
        cout << "No card present\n";
        return this;
    }

    ATMState* enterPIN(ATM* atm, int pin) {
        cout << "ATM out of cash\n";
        return this;
    }

    ATMState* withdrawCash(ATM* atm, int amount) {
        cout << "ATM out of cash\n";
        return this;
    }

    string getStateName() { return "NO_CASH"; }
};

// Constructor
ATM::ATM(int cash, int pin) {
    this->cash = cash;
    this->correctPIN = pin;

    noCardState = new NoCardState();
    hasCardState = new HasCardState();
    authenticatedState = new AuthenticatedState();
    noCashState = new NoCashState();

    if (cash > 0)
        currentState = noCardState;
    else
        currentState = noCashState;
}

// MAIN
int main() {
    ATM atm(5000, 1234);

    atm.printStatus();

    atm.insertCard();
    atm.enterPIN(1234);
    atm.withdrawCash(2000);

    atm.printStatus();

    atm.insertCard();
    atm.enterPIN(1111);
    atm.withdrawCash(1000);

    atm.ejectCard();

    atm.printStatus();

    return 0;
}