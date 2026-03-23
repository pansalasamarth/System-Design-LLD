#include <iostream>
using namespace std;

class NumberAnalyzer {
private:
    int number;

public:
    NumberAnalyzer(int n) {
        number = n;
    }

    bool isEven() {
        int remainder = number % 2;

        if(remainder == 0) {
            return true;
        }
        else {
            return false;
        }
    }
};

int main() {
    NumberAnalyzer analyzer(10);

    if(analyzer.isEven()) {
        cout << "Even number";
    }
}