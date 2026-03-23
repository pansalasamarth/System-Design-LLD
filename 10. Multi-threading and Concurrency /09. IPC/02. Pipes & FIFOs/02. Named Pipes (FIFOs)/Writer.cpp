#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace std;

int main() {
    const char* fifo_path = "/tmp/myfifo";

    int fd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    const char* msg = "Hello via FIFO!";
    write(fd, msg, strlen(msg) + 1); // include null terminator

    close(fd);
    return 0;
}