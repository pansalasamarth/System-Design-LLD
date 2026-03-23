#include <iostream>
using namespace std;

class PaymentGateway {
private:
    static PaymentGateway* instance;

    string apiKey;

    // Private constructor
    PaymentGateway() {
        apiKey = "PAYMENT_API_KEY_123";
        cout << "Connecting to Payment Gateway using API Key...\n";
    }

public:
    // Singleton access
    static PaymentGateway* getInstance() {
        if(instance == nullptr) {
            instance = new PaymentGateway();
        }
        return instance;
    }

    void makePayment(string user, double amount) {
        cout << "User: " << user << " paid Rs " << amount << " successfully.\n";
    }
};

// Initialize static variable
PaymentGateway* PaymentGateway::instance = nullptr;

int main() {

    // First payment
    PaymentGateway* gateway1 = PaymentGateway::getInstance();
    gateway1->makePayment("Samarth", 1500);

    // Second payment
    PaymentGateway* gateway2 = PaymentGateway::getInstance();
    gateway2->makePayment("Ansh", 2200);

    // Check if both objects are same
    cout << "Same Gateway Instance? " << (gateway1 == gateway2) << endl;

    return 0;
}