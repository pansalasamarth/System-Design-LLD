#include <iostream>
using namespace std;

// Target Interface
class PaymentGateway {
public:
    virtual void pay(int amount) = 0;
};

// Adaptee 1 (PayPal API)
class PayPalAPI {
public:
    void makePayment(int amount) {
        cout << "Payment of Rs " << amount << " done using PayPal" << endl;
    }
};

// Adaptee 2 (Stripe API)
class StripeAPI {
public:
    void charge(int amount) {
        cout << "Payment of Rs " << amount << " done using Stripe" << endl;
    }
};

// Adapter for PayPal
class PayPalAdapter : public PaymentGateway {
private:
    PayPalAPI* paypal;

public:
    PayPalAdapter(PayPalAPI* p) {
        paypal = p;
    }

    void pay(int amount) {
        paypal->makePayment(amount);
    }
};

// Adapter for Stripe
class StripeAdapter : public PaymentGateway {
private:
    StripeAPI* stripe;

public:
    StripeAdapter(StripeAPI* s) {
        stripe = s;
    }

    void pay(int amount) {
        stripe->charge(amount);
    }
};

int main() {

    PayPalAPI* paypal = new PayPalAPI();
    StripeAPI* stripe = new StripeAPI();

    PaymentGateway* payment1 = new PayPalAdapter(paypal);
    PaymentGateway* payment2 = new StripeAdapter(stripe);

    payment1->pay(1000);
    payment2->pay(2000);

    return 0;
}