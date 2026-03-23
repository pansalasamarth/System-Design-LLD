#include<iostream>

using namespace std;

// Sub class methods should not be allowed state changes What
// Base class never allowed.

class BankAccount {
protected:
    double balance;

public:
    BankAccount(double b) {
        if (b < 0) throw invalid_argument("Balance can't be negative");
        balance = b;
    }

    // History Constraint : Withdraw should be allowed
    virtual void withdraw(double amount) {
        if (balance - amount < 0) throw runtime_error("Insufficient funds");
        balance -= amount;
        cout<< "Amount withdrawn. Remaining balance is " << balance << endl;
    }
};
    

class FixedDepositAccount : public BankAccount {
public:
    FixedDepositAccount(double b) : BankAccount(b) {}

    // LSP break! History constraint broke!
    // Parent class behaviour change : Now withdraw is not allowed.
    //This class will brake client code that relies on withdraw.
    void withdraw(double amount) override {
        throw runtime_error("Withdraw not allowed in Fixed Deposit");
    }
};
    
int main() {
    BankAccount* bankAccount = new BankAccount(100);
    bankAccount->withdraw(100);
}