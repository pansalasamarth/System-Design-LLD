// Strategy Interface
class PaymentStrategy {
public:
    virtual void pay(int amount) = 0;
};

// Strategy Interface
#include <iostream>
using namespace std;

class CreditCardPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Paid " << amount << " using Credit Card" << endl;
    }
};

class UPIPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Paid " << amount << " using UPI" << endl;
    }
};

// Context Class
class PaymentContext {
private:
    PaymentStrategy* strategy;

public:
    PaymentContext(PaymentStrategy* s) {
        strategy = s;
    }

    void makePayment(int amount) {
        strategy->pay(amount);
    }
};

// Client Code
int main() {

    CreditCardPayment creditCard;
    UPIPayment upi;

    PaymentContext payment1(&creditCard);
    payment1.makePayment(1000);

    PaymentContext payment2(&upi);
    payment2.makePayment(500);

    return 0;
}