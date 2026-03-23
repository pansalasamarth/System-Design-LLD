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

// Null Object
class NullCustomer : public Customer {
public:
    string getName() {
        return "No Customer";
    }
};

Customer* getCustomer(bool exists) {
    if (exists)
        return new RealCustomer("Samarth");
    else
        return new NullCustomer();  // no NULL
}

int main() {
    Customer* c = getCustomer(false);

    // No NULL check needed
    cout << c->getName() << endl;

    return 0;
}