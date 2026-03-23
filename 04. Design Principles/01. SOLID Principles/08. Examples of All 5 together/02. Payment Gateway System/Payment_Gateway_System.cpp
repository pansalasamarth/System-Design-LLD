#include <iostream>
#include <string>

using namespace std;

// ==============================
// Payment Request Model
// ==============================

class PaymentRequest {
public:
    string user;
    double amount;

    PaymentRequest(string u, double a) {
        user = u;
        amount = a;
    }
};

// ==============================
// Interface for Payment Processing
// ==============================

class PaymentProcessor {
public:
    virtual bool process(PaymentRequest request) = 0;
};

// ==============================
// Credit Card Processor
// ==============================

class CardProcessor : public PaymentProcessor {
public:
    bool process(PaymentRequest request) override {
        cout << "Processing Card payment for "
             << request.user
             << " amount: "
             << request.amount
             << endl;

        return true;
    }
};

// ==============================
// UPI Processor
// ==============================

class UPIProcessor : public PaymentProcessor {
public:
    bool process(PaymentRequest request) override {
        cout << "Processing UPI payment for "
             << request.user
             << " amount: "
             << request.amount
             << endl;

        return true;
    }
};

// ==============================
// Fraud Detection (SRP)
// ==============================

class FraudDetector {
public:
    bool checkFraud(PaymentRequest request) {

        if(request.amount > 100000) {
            cout << "Fraud detected!" << endl;
            return false;
        }

        return true;
    }
};

// ==============================
// Logger
// ==============================

class Logger {
public:
    void log(string message) {
        cout << "[LOG] " << message << endl;
    }
};

// ==============================
// Receipt Generator
// ==============================

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

// ==============================
// Payment Gateway (High Level)
// ==============================

class PaymentGateway {

private:

    PaymentProcessor* processor;
    FraudDetector fraudDetector;
    Logger logger;
    ReceiptService receiptService;

public:

    PaymentGateway(PaymentProcessor* p) {
        processor = p;
    }

    void executePayment(PaymentRequest request) {

        logger.log("Payment request received");

        if(!fraudDetector.checkFraud(request)) {
            logger.log("Payment blocked due to fraud");
            return;
        }

        bool success = processor->process(request);

        if(success) {
            logger.log("Payment successful");
            receiptService.generateReceipt(request);
        }
        else {
            logger.log("Payment failed");
        }
    }
};

// ==============================
// Main
// ==============================

int main() {

    CardProcessor card;
    UPIProcessor upi;

    PaymentRequest request1("Samarth", 5000);
    PaymentRequest request2("Ansh", 1200);

    PaymentGateway gateway1(&card);
    gateway1.executePayment(request1);

    cout << endl;

    PaymentGateway gateway2(&upi);
    gateway2.executePayment(request2);

}