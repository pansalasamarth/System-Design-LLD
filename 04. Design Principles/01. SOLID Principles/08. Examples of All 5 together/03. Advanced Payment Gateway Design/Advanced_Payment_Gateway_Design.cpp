#include <iostream>
#include <map>
#include <string>

using namespace std;

// ==============================
// Transaction State
// ==============================

enum TransactionState {
    CREATED,
    PROCESSING,
    SUCCESS,
    FAILED,
    REFUNDED
};

// ==============================
// Payment Request
// ==============================

class PaymentRequest {
public:
    string id;
    string user;
    double amount;

    PaymentRequest(string id, string user, double amount) {
        this->id = id;
        this->user = user;
        this->amount = amount;
    }
};

// ==============================
// Payment Processor Interface
// ==============================

class PaymentProcessor {
public:
    virtual bool process(PaymentRequest request) = 0;
};

// ==============================
// Card Processor
// ==============================

class CardProcessor : public PaymentProcessor {
public:
    bool process(PaymentRequest request) override {

        cout << "Card payment processing for "
             << request.user << endl;

        return true;
    }
};

// ==============================
// UPI Processor
// ==============================

class UPIProcessor : public PaymentProcessor {
public:
    bool process(PaymentRequest request) override {

        cout << "UPI payment processing for "
             << request.user << endl;

        return true;
    }
};

// ==============================
// Fraud Detection
// ==============================

class FraudDetector {
public:

    bool check(PaymentRequest request) {

        if(request.amount > 50000) {
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

    void log(string msg) {
        cout << "[LOG] " << msg << endl;
    }
};

// ==============================
// Transaction Database
// ==============================

class TransactionDB {

private:
    map<string, TransactionState> transactions;

public:

    void save(string id, TransactionState state) {
        transactions[id] = state;
    }

    TransactionState get(string id) {
        return transactions[id];
    }
};

// ==============================
// Retry Service
// ==============================

class RetryService {

public:

    bool retry(PaymentProcessor* processor, PaymentRequest request) {

        cout << "Retrying payment..." << endl;

        return processor->process(request);
    }
};

// ==============================
// Refund Service
// ==============================

class RefundService {

public:

    void refund(string user, double amount) {

        cout << "Refunding " << amount
             << " to " << user << endl;
    }
};

// ==============================
// Payment Gateway
// ==============================

class PaymentGateway {

private:

    PaymentProcessor* processor;
    FraudDetector fraudDetector;
    Logger logger;
    TransactionDB db;
    RetryService retryService;
    RefundService refundService;

public:

    PaymentGateway(PaymentProcessor* p) {
        processor = p;
    }

    void executePayment(PaymentRequest request) {

        logger.log("Payment started");

        db.save(request.id, CREATED);

        if(!fraudDetector.check(request)) {
            db.save(request.id, FAILED);
            return;
        }

        db.save(request.id, PROCESSING);

        bool success = processor->process(request);

        if(!success) {
            success = retryService.retry(processor, request);
        }

        if(success) {
            db.save(request.id, SUCCESS);
            logger.log("Payment successful");
        }
        else {
            db.save(request.id, FAILED);
            logger.log("Payment failed");
        }
    }

    void refundPayment(PaymentRequest request) {

        if(db.get(request.id) == SUCCESS) {

            refundService.refund(request.user, request.amount);
            db.save(request.id, REFUNDED);

            logger.log("Payment refunded");
        }
    }
};

// ==============================
// Main
// ==============================

int main() {

    CardProcessor card;
    UPIProcessor upi;

    PaymentGateway gateway(&card);

    PaymentRequest request("TXN1001","Samarth",2000);

    gateway.executePayment(request);

    gateway.refundPayment(request);

}