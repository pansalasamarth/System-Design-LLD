#include <iostream>

using namespace std;

// Return Type Rule : 
// Subtype overriden method return type should be either identical 
// or narrower then the parent method's return type.
// This is also called as return type covariance.
// C++ enforces this by covariance.

class Animal {
    //some common Animal methods
};

class Dog : public Animal {
    //Additional Dog methods specific to Dogs.
};


class Parent {
public:
    virtual Animal* getAnimal() { 
        cout << "Parent : Returning Animal instance" << endl;
        return new Animal();
    }
};

class Child : public Parent {
public:
//  Can also have return type as Dog
    Animal* getAnimal() override { 
        cout << "Child : Returning Dog instance" << std::endl;
        return new Dog();
    }
};

class Client {
private:
    Parent* p;

public:
    Client(Parent* p) {
        this->p = p;
    }
    void takeAnimal() {
        p->getAnimal();
    }
};

int main() {
    Parent* parent = new Parent();
    Child* child = new Child();

    Client* client = new Client(child);
    //Client * client = new Client(parent);
    client->takeAnimal();

    return 0;
}