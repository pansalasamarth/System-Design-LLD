#include <iostream>
#include <string>

using namespace std;

// Forward declaration
class VendingMachine;

// Forward declarations for concrete states
class NoCoinState;
class HasCoinState;
class DispenseState;
class SoldOutState;

// Abstract State Interface
class VendingState {
public:
    virtual VendingState* insertCoin(VendingMachine* machine, int coin) = 0;
    virtual VendingState* selectItem(VendingMachine* machine) = 0;
    virtual VendingState* dispense(VendingMachine* machine) = 0;
    virtual VendingState* returnCoin(VendingMachine* machine) = 0;
    virtual VendingState* refill(VendingMachine* machine, int quantity) = 0;
    virtual string getStateName() = 0;
};

// Context Class - Vending Machine
class VendingMachine {
private:
    VendingState* currentState;
    int itemCount;
    int itemPrice;
    int  insertedCoins;
    
    // State objects (we'll initialize these)
    VendingState* noCoinState;
    VendingState* hasCoinState;
    VendingState* dispenseState;
    VendingState* soldOutState;
    
public:
    VendingMachine(int itemCount, int itemPrice);
    
    // Delegate to current state and update state based on return value
    void insertCoin(int coin);
    void selectItem();
    void dispense();
    void returnCoin();
    void refill(int quantity);
        
    // Print the status of Vending Machine
    void printStatus();
    
    // Getters for states
    VendingState* getNoCoinState() { 
        return noCoinState;
    }
    VendingState* getHasCoinState() { 
        return hasCoinState;
    }
    VendingState* getDispenseState() { 
        return dispenseState; 
    }
    VendingState* getSoldOutState() { 
        return soldOutState;
    }
    
    // Data access methods
    int getItemCount() { 
        return itemCount; 
    }
    void decrementItemCount() { 
        itemCount--; 
    }
    void incrementItemCount(int count = 1) {
        itemCount += count;
    }
    int getInsertedCoin() { 
        return insertedCoins;
    }
    void setInsertedCoin(int coin) { 
        insertedCoins = coin;
    }
    void addCoin(int coin) { 
        insertedCoins += coin;
    }
    int getPrice() {
        return this->itemPrice;
    }
    void setPrice(int itemPrice) {
        this->itemPrice = itemPrice;
    }
};

// Concrete State: No Coin Inserted
class NoCoinState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
        machine->setInsertedCoin(coin); // Rs 10
        cout << "Coin inserted. Current balance: Rs " << coin <<endl;
        return machine->getHasCoinState(); // Transition to HasCoinState
    }
    
    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Please insert coin first!" <<endl;
        return machine->getNoCoinState(); // Stay in same state
    }
    
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Please insert coin and select item first!" <<endl;
        return machine->getNoCoinState(); // Stay in same state
    }
    
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "No coin to return!" <<endl;
        return machine->getNoCoinState(); // Stay in same state
    }

    VendingState* refill(VendingMachine* machine, int quantity) override {
        cout << "Items refilling" <<endl;
        machine->incrementItemCount(quantity);
        return machine->getNoCoinState(); // Stay in same state
    }
    
   string getStateName() override {
        return "NO_COIN";
    }
};

// Concrete State: Coin Inserted
class HasCoinState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
        machine->addCoin(coin);
        cout << "Additional coin inserted. Current balance: Rs " << machine->getInsertedCoin() <<endl;
        return machine->getHasCoinState(); // Stay in same state
    }
    
    VendingState* selectItem(VendingMachine* machine) override {
        if (machine->getInsertedCoin() >= machine->getPrice()) {
           cout << "Item selected. Dispensing..." <<endl;
            
            int change = machine->getInsertedCoin() - machine->getPrice();
            if (change > 0) {
               cout << "Change returned: Rs " << change <<endl;
            }
            machine->setInsertedCoin(0);
            
            return machine->getDispenseState(); // Transition to DispenseState
        } 
        else {
            int needed = machine->getPrice() - machine->getInsertedCoin();
            cout << "Insufficient funds. Need Rs " << needed << " more." <<endl;
            return machine->getHasCoinState(); // Stay in same state
        }
    }
    
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Please select an item first!" <<endl;
        return machine->getHasCoinState(); // Stay in same state
    }
    
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Coin returned: Rs " << machine->getInsertedCoin() <<endl;
        machine->setInsertedCoin(0);
        return machine->getNoCoinState(); // Transition to NoCoinState
    }

    VendingState* refill(VendingMachine* machine, int quantity) override {
        cout << "Can't refil in this state" <<endl;
        return machine->getHasCoinState(); // Stay in same state
    }
    
   string getStateName() override {
        return "HAS_COIN";
    }
};

// Concrete State: Item Sold
class DispenseState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
       cout << "Please wait, already dispensing item. Coin returned: Rs " << coin <<endl;
        return machine->getDispenseState();  // Stay in same state
    }
    
    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Already dispensing item. Please wait." <<endl;
        return machine->getDispenseState(); // Stay in same state
    }
    
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Item dispensed!" <<endl;
        machine->decrementItemCount();
        
        if (machine->getItemCount() > 0) {
            return machine->getNoCoinState(); // Transition to NoCoinState
        } 
        else {
           cout << "Machine is now sold out!" <<endl;
            return machine->getSoldOutState(); // Transition to SoldOutState
        }
    }
    
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Cannot return coin while dispensing item!" <<endl;
        return machine->getDispenseState(); // Stay in same state
    }

    VendingState* refill(VendingMachine* machine, int quantity) override {
        cout << "Can't refil in this state" <<endl;
        return machine->getDispenseState(); // Stay in same state
    }

   string getStateName() override {
        return "DISPENSING";
    }
};

// Concrete State: Sold Out
class SoldOutState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* machine, int coin) override {
       cout << "Machine is sold out. Coin returned: Rs " << coin <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }
    
    VendingState* selectItem(VendingMachine* machine) override {
       cout << "Machine is sold out!" <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }
    
    VendingState* dispense(VendingMachine* machine) override {
       cout << "Machine is sold out!" <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }
    
    VendingState* returnCoin(VendingMachine* machine) override {
       cout << "Machine is sold out. No coin inserted." <<endl;
        return machine->getSoldOutState(); // Stay in same state
    }

    VendingState* refill(VendingMachine* machine, int quantity) override {
        cout << "Items refilling" <<endl;
        machine->incrementItemCount(quantity);
        return machine->getNoCoinState();
    }
    
   string getStateName() override {
        return "SOLD_OUT";
    }
};

// VendingMachine implementation (after all classes are defined)
VendingMachine::VendingMachine(int itemCount, int itemPrice) {

    this->itemCount = itemCount;
    this->itemPrice = itemPrice;
    this->insertedCoins = 0; 
    
    // Create state objects
    noCoinState = new NoCoinState();
    hasCoinState = new HasCoinState();
    dispenseState = new DispenseState();
    soldOutState = new SoldOutState();
    
    // Set initial state
    if (itemCount  > 0) {
        currentState = noCoinState;
    } else {
        currentState = soldOutState;
    }
}

void VendingMachine::insertCoin(int coin) {
    currentState = currentState->insertCoin(this, coin);
}

void VendingMachine::selectItem() {
    currentState = currentState->selectItem(this);
}

void VendingMachine::dispense() {
    currentState = currentState->dispense(this);
}

void VendingMachine::returnCoin() {
    currentState = currentState->returnCoin(this);
}

void VendingMachine::refill(int quantity) {
    currentState = currentState->refill(this, quantity);
}

void VendingMachine::printStatus() {
    cout << "\n--- Vending Machine Status ---" << endl;
    cout << "Items remaining: " << itemCount << endl;
    cout << "Inserted coin: Rs " << insertedCoins << endl;
    cout << "Current state: " << currentState->getStateName() << endl << endl;
}

int main() {
    cout << "=== Water Bottle VENDING MACHINE ===" <<endl;
    
    int itemCount = 2;
    int itemPrice = 20;

    VendingMachine machine(itemCount, itemPrice);
    machine.printStatus();
    
    // Test scenarios - each operation potentially changes state
    cout << "1. Trying to select item without coin:" <<endl;
    machine.selectItem();  // Should ask for coin, no state change
    machine.printStatus();
    
    cout << "2. Inserting coin:" <<endl;
    machine.insertCoin(10);  // State changes to HAS_COIN
    machine.printStatus();
    
    cout << "3. Selecting item with insufficient funds:" <<endl;
    machine.selectItem();  // Insufficient funds, stays in HAS_COIN
    machine.printStatus();
    
    cout << "4. Adding more coins:" <<endl;
    machine.insertCoin(10);  // Add more money, stays in HAS_COIN
    machine.printStatus();
    
    cout << "5. Selecting item Now" <<endl;
    machine.selectItem();  // State changes to SOLD
    machine.printStatus();
    
    cout << "6. Dispensing item:" <<endl;
    machine.dispense(); // State changes to NO_COIN (items remaining)
    machine.printStatus();
    
    cout << "7. Buying last item:" <<endl;
    machine.insertCoin(20);  // State changes to HAS_COIN
    machine.selectItem();  // State changes to SOLD
    machine.dispense(); // State changes to SOLD_OUT (no items left)
    machine.printStatus();
    
    cout << "8. Trying to use sold out machine:" <<endl;
    machine.insertCoin(5);  // Coin returned, stays in SOLD_OUT

    cout << "9. Trying to use sold out machine:" <<endl;
    machine.refill(2);
    machine.printStatus(); // State changes NO_COIN
    
    return 0;
}