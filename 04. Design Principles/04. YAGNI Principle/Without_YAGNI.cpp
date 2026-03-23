#include <iostream>
#include <string>
using namespace std;

// Over-designed payment interface
class PaymentProcessor {
public:
    virtual void processPayment(double amount) = 0;
    virtual void refund(double amount) = 0;
    virtual void schedulePayment(double amount, string date) = 0;
    virtual void recurringPayment(double amount, int months) = 0;
    virtual void generateAnalytics() = 0;
};

// Only UPI payment is actually required right now
class UPIPayment : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "Processing UPI payment of " << amount << endl;
    }

    // Unnecessary implementations (not required yet)
    void refund(double amount) override {
        cout << "Refund feature not implemented yet" << endl;
    }

    void schedulePayment(double amount, string date) override {
        cout << "Scheduled payment feature not implemented yet" << endl;
    }

    void recurringPayment(double amount, int months) override {
        cout << "Recurring payment feature not implemented yet" << endl;
    }

    void generateAnalytics() override {
        cout << "Analytics feature not implemented yet" << endl;
    }
};

int main() {
    UPIPayment payment;
    payment.processPayment(2000);
}