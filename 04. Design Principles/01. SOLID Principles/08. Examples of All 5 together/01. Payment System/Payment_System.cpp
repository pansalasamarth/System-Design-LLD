#include <iostream>
using namespace std;

// ==============================
// Abstraction (ISP + DIP)
// ==============================

class PaymentProcessor {
public:
    virtual void processPayment(double amount) = 0;
};

class RefundProcessor {
public:
    virtual void refund(double amount) = 0;
};

// ==============================
// Concrete Payment Methods
// ==============================

class CreditCardPayment : public PaymentProcessor, public RefundProcessor {
public:
    void processPayment(double amount) override {
        cout << "Processing Credit Card payment of $" << amount << endl;
    }

    void refund(double amount) override {
        cout << "Refunding $" << amount << " to Credit Card" << endl;
    }
};

class UPIPayment : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "Processing UPI payment of $" << amount << endl;
    }
};

class PayPalPayment : public PaymentProcessor, public RefundProcessor {
public:
    void processPayment(double amount) override {
        cout << "Processing PayPal payment of $" << amount << endl;
    }

    void refund(double amount) override {
        cout << "Refunding $" << amount << " via PayPal" << endl;
    }
};

// ==============================
// SRP: Separate class for receipt
// ==============================

class ReceiptGenerator {
public:
    void generate(double amount) {
        cout << "Receipt Generated for payment of $" << amount << endl;
    }
};

// ==============================
// DIP: PaymentService depends on abstraction
// ==============================

class PaymentService {
private:
    PaymentProcessor* processor;
    ReceiptGenerator receipt;

public:
    PaymentService(PaymentProcessor* p) {
        processor = p;
    }

    void makePayment(double amount) {
        processor->processPayment(amount);
        receipt.generate(amount);
    }
};

// ==============================
// Main
// ==============================

int main() {

    CreditCardPayment creditCard;
    UPIPayment upi;
    PayPalPayment paypal;

    PaymentService service1(&creditCard);
    service1.makePayment(1000);

    PaymentService service2(&upi);
    service2.makePayment(500);

    PaymentService service3(&paypal);
    service3.makePayment(750);

}