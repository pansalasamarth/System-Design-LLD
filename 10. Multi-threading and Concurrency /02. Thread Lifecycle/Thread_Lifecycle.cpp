#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void task() {
    cout << "Thread started\n";

    this_thread::sleep_for(chrono::seconds(2)); // Timed Waiting

    cout << "Thread finished\n";
}

int main() {

    thread t(task);  // New → Runnable

    t.join();        // Running → Terminated

    cout << "Main thread ends\n";

    return 0;
}