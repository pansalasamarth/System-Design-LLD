#include <iostream>
using namespace std;

// Strategy Interface
class PaymentStrategy {
public:
    virtual void pay(int amount) = 0;
    virtual ~PaymentStrategy() {}
};

// Concrete Strategy 1 - Credit Card Payment
class CreditCardPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Payment of " << amount << " done using Credit Card." << endl;
    }
};

// Concrete Strategy 2 - UPI Payment
class UPIPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Payment of " << amount << " done using UPI." << endl;
    }
};

// Concrete Strategy 3 - PayPal Payment
class PayPalPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Payment of " << amount << " done using PayPal." << endl;
    }
};

// Concrete Strategy 4 - Stripe Payment
class StripePayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Payment of " << amount << " done using Stripe." << endl;
    }
};

// Context Class
class PaymentGateway {
private:
    PaymentStrategy* paymentMethod;

public:
    PaymentGateway(PaymentStrategy* method) {
        paymentMethod = method;
    }

    void processPayment(int amount) {
        paymentMethod->pay(amount);
    }
};

// Client Code
int main() {

    CreditCardPayment creditCard;
    UPIPayment upi;
    PayPalPayment paypal;
    StripePayment stripe;

    PaymentGateway gateway1(&creditCard);
    gateway1.processPayment(1000);

    PaymentGateway gateway2(&upi);
    gateway2.processPayment(500);

    PaymentGateway gateway3(&paypal);
    gateway3.processPayment(2000);

    PaymentGateway gateway4(&stripe);
    gateway4.processPayment(5000);

    return 0;
}
