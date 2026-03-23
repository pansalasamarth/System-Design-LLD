#include <iostream>
using namespace std;

class Customer {
public:
    virtual string getName() = 0;
};

class RealCustomer : public Customer {
    string name;
public:
    RealCustomer(string n) { name = n; }
    string getName() { return name; }
};

Customer* getCustomer(bool exists) {
    if (exists)
        return new RealCustomer("Samarth");
    else
        return NULL;  // risky
}

int main() {
    Customer* c = getCustomer(false);

    // Must check every time
    if (c != NULL) {
        cout << c->getName() << endl;
    } else {
        cout << "Customer not found\n";
    }
}