#include <iostream>
using namespace std;

class Bank {
public:
    void makePayment(int amount) {
        cout << "Payment of " << amount << " processed by Bank" << endl;
    }
};

class Wallet {
public:
    Bank bank;
};

class Customer {
public:
    Wallet wallet;

    void pay(int amount) {
        // Violates Law of Demeter
        wallet.bank.makePayment(amount);
    }
};

int main() {
    Customer customer;
    customer.pay(500);
}