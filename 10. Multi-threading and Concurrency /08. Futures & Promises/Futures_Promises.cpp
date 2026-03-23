#include <iostream>
#include <thread>
#include <future>
using namespace std;

void compute(promise<int> p) {
    int result = 10 + 20;
    p.set_value(result);
}

int main() {
    promise<int> p;
    future<int> f = p.get_future();

    thread t(compute, move(p));

    cout << "Waiting for result...\n";
    cout << "Result: " << f.get() << endl;

    t.join();
    return 0;
}