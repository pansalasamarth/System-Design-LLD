#include <iostream>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;

int main() {
    const char* fifo_path = "/tmp/myfifo";

    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    cout << "FIFO created at " << fifo_path << endl;
    return 0;
}