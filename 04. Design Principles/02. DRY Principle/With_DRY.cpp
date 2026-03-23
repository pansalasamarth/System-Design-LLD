#include <iostream>
using namespace std;

class TaxCalculator {
public:
    static double calculateTotal(double price) {
        double tax = price * 0.18;
        return price + tax;
    }
};

class OrderService {
public:
    void processOnlineOrder(double price) {
        cout << "Online Order Total: "
             << TaxCalculator::calculateTotal(price)
             << endl;
    }

    void processStoreOrder(double price) {
        cout << "Store Order Total: "
             << TaxCalculator::calculateTotal(price)
             << endl;
    }
};

int main() {
    OrderService service;
    service.processOnlineOrder(1000);
    service.processStoreOrder(500);
}