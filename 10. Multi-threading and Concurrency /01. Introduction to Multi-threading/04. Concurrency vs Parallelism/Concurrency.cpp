#include <iostream>
#include <thread>
using namespace std;

void task(string name) {
    for(int i = 0; i < 3; i++) {
        cout << name << " executing\n";
    }
}

int main() {

    thread t1(task, "Task A");
    thread t2(task, "Task B");

    t1.join();
    t2.join();

    return 0;
}