#include <iostream>
#include <string>
using namespace std;

// Over-designed payment interface
class PaymentProcessor {
public:
    virtual void processPayment(double amount) = 0;
};

// Only UPI payment is actually required right now
class UPIPayment : public PaymentProcessor {
public:
    void processPayment(double amount) override {
        cout << "Processing UPI payment of " << amount << endl;
    }
};

int main() {
    UPIPayment payment;
    payment.processPayment(2000);
}