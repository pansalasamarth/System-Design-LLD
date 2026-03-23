#include <iostream>
using namespace std;

// Abstract Expression
class Expression {
public:
    virtual int interpret() = 0;
    virtual ~Expression() {}
};

// Terminal Expression (Number)
class Number : public Expression {
private:
    int value;

public:
    Number(int v) {
        value = v;
    }

    int interpret() override {
        return value;
    }
};

// Non-Terminal Expression (Addition)
class Add : public Expression {
private:
    Expression* left;
    Expression* right;

public:
    Add(Expression* l, Expression* r) {
        left = l;
        right = r;
    }

    int interpret() override {
        return left->interpret() + right->interpret();
    }
};

// Non-Terminal Expression (Subtraction)
class Subtract : public Expression {
private:
    Expression* left;
    Expression* right;

public:
    Subtract(Expression* l, Expression* r) {
        left = l;
        right = r;
    }

    int interpret() override {
        return left->interpret() - right->interpret();
    }
};

int main() {
    // Expression: (5 + 3) - 2
    Expression* expr = new Subtract(
        new Add(
            new Number(5),
            new Number(3)
        ),
        new Number(2)
    );

    cout << "Result = " << expr->interpret() << endl;

    return 0;
}