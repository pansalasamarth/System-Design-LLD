#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

int main() {
    const char* fifo_path = "/tmp/myfifo";

    int fd = open(fifo_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    char buf[100];
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n > 0) {
        buf[n] = '\0'; // ensure null termination
        cout << "Received: " << buf << endl;
    }

    close(fd);
    return 0;
}