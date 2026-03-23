#include <iostream>
using namespace std;

class OrderService {
public:
    void processOnlineOrder(double price) {
        double tax = price * 0.18;
        double total = price + tax;
        cout << "Online Order Total: " << total << endl;
    }

    void processStoreOrder(double price) {
        double tax = price * 0.18;
        double total = price + tax;
        cout << "Store Order Total: " << total << endl;
    }
};

int main() {
    OrderService service;
    service.processOnlineOrder(1000);
    service.processStoreOrder(500);
}