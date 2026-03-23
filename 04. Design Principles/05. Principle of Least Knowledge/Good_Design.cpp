#include <iostream>
using namespace std;

class Bank {
public:
    void makePayment(int amount) {
        cout << "Payment of " << amount << " processed by Bank" << endl;
    }
};

class Wallet {
private:
    Bank bank;

public:
    void pay(int amount) {
        bank.makePayment(amount);
    }
};

class Customer {
private:
    Wallet wallet;

public:
    void pay(int amount) {
        // Customer only talks to Wallet
        wallet.pay(amount);
    }
};

int main() {
    Customer customer;
    customer.pay(500);
}