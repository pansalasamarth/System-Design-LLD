#include <iostream>
using namespace std;

// Component
class Coffee {
public:
    virtual string getDescription() = 0;
    virtual int cost() = 0;
};

// Concrete Component
class SimpleCoffee : public Coffee {
public:
    string getDescription() {
        return "Simple Coffee";
    }

    int cost() {
        return 50;
    }
};

// Decorator
class CoffeeDecorator : public Coffee {
protected:
    Coffee* coffee;
public:
    CoffeeDecorator(Coffee* c) {
        coffee = c;
    }
};

// Concrete Decorator - Milk
class Milk : public CoffeeDecorator {
public:
    Milk(Coffee* c) : CoffeeDecorator(c) {}

    string getDescription() {
        return coffee->getDescription() + " + Milk";
    }

    int cost() {
        return coffee->cost() + 10;
    }
};

// Concrete Decorator - Sugar
class Sugar : public CoffeeDecorator {
public:
    Sugar(Coffee* c) : CoffeeDecorator(c) {}

    string getDescription() {
        return coffee->getDescription() + " + Sugar";
    }

    int cost() {
        return coffee->cost() + 5;
    }
};

int main() {

    Coffee* order = new SimpleCoffee();

    order = new Milk(order);
    order = new Sugar(order);

    cout << "Order: " << order->getDescription() << endl;
    cout << "Total Cost: Rs " << order->cost() << endl;

    return 0;
}