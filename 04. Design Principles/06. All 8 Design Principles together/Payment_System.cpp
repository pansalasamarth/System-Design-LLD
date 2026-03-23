#include <iostream>
#include <string>
using namespace std;

// =============================
// DRY: Reusable Logger Utility
// =============================
class Logger {
public:
    static void log(string msg) {
        cout << "[LOG] " << msg << endl;
    }
};

// =============================
// Payment Request Model
// =============================
class PaymentRequest {
public:
    string user;
    double amount;

    PaymentRequest(string user, double amount) {
        this->user = user;
        this->amount = amount;
    }
};

// =============================
// ISP + DIP: Payment Interface
// =============================
class PaymentProcessor {
public:
    virtual void processPayment(PaymentRequest request) = 0;
};

// =============================
// Credit Card Payment
// =============================
class CreditCardPayment : public PaymentProcessor {
public:
    void processPayment(PaymentRequest request) override {

        Logger::log("Processing Credit Card Payment");

        cout << "User: " << request.user << endl;
        cout << "Amount: " << request.amount << endl;
    }
};

// =============================
// UPI Payment
// =============================
class UPIPayment : public PaymentProcessor {
public:
    void processPayment(PaymentRequest request) override {

        Logger::log("Processing UPI Payment");

        cout << "User: " << request.user << endl;
        cout << "Amount: " << request.amount << endl;
    }
};

// =============================
// PayPal Payment
// =============================
class PayPalPayment : public PaymentProcessor {
public:
    void processPayment(PaymentRequest request) override {

        Logger::log("Processing PayPal Payment");

        cout << "User: " << request.user << endl;
        cout << "Amount: " << request.amount << endl;
    }
};

// =============================
// SRP: Fraud Detection
// =============================
class FraudDetector {
public:
    bool checkFraud(PaymentRequest request) {

        if(request.amount > 50000) {
            Logger::log("Fraud detected!");
            return false;
        }

        return true;
    }
};

// =============================
// SRP: Receipt Service
// =============================
class ReceiptService {
public:
    void generateReceipt(PaymentRequest request) {

        cout << "Receipt generated for "
             << request.user
             << " amount: "
             << request.amount
             << endl;
    }
};

// =============================
// DIP: High Level Module
// =============================
class PaymentService {

private:

    PaymentProcessor* processor;
    FraudDetector fraudDetector;
    ReceiptService receiptService;

public:

    PaymentService(PaymentProcessor* processor) {
        this->processor = processor;
    }

    void makePayment(PaymentRequest request) {

        Logger::log("Payment initiated");

        if(!fraudDetector.checkFraud(request)) {
            Logger::log("Payment blocked");
            return;
        }

        processor->processPayment(request);

        receiptService.generateReceipt(request);

        Logger::log("Payment completed");
    }
};

// =============================
// Main
// =============================
int main() {

    PaymentRequest request1("Samarth", 2000);
    PaymentRequest request2("Ansh", 3500);
    PaymentRequest request3("Maulik", 1000);

    CreditCardPayment creditCard;
    UPIPayment upi;
    PayPalPayment paypal;

    PaymentService service1(&creditCard);
    service1.makePayment(request1);

    cout << endl;

    PaymentService service2(&upi);
    service2.makePayment(request2);

    cout << endl;

    PaymentService service3(&paypal);
    service3.makePayment(request3);

    return 0;
}