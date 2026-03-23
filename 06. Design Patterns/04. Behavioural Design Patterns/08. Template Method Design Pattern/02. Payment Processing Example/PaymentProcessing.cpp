#include <iostream>
using namespace std;

// Base Class (Template)
class PaymentProcessor {
public:

    // Template Method
    void processPayment(double amount) {
        validatePayment(amount);
        authenticateUser();
        makePayment(amount);
        generateReceipt(amount);
    }

protected:

    void validatePayment(double amount) {
        cout << "Validating payment of Rs " << amount << endl;
    }

    void authenticateUser() {
        cout << "Authenticating user..." << endl;
    }

    virtual void makePayment(double amount) = 0;

    void generateReceipt(double amount) {
        cout << "Generating receipt for Rs " << amount << endl;
    }
};

// Credit Card Payment
class CreditCardPayment : public PaymentProcessor {
protected:
    void makePayment(double amount) override {
        cout << "Processing Credit Card payment of Rs " << amount << endl;
    }
};

// UPI Payment
class UPIPayment : public PaymentProcessor {
protected:
    void makePayment(double amount) override {
        cout << "Processing UPI payment of Rs " << amount << endl;
    }
};

int main() {

    PaymentProcessor* payment1 = new CreditCardPayment();
    payment1->processPayment(2000);

    cout << endl;

    PaymentProcessor* payment2 = new UPIPayment();
    payment2->processPayment(500);

    return 0;
}