#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

using namespace std;

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // Child process
        close(pipefd[1]); // Close write end
        char buf[100];
        ssize_t n = read(pipefd[0], buf, sizeof(buf));
        if (n > 0) {
            buf[n] = '\0'; // Ensure null-termination
            cout << "Child received: " << buf << endl;
        }
        close(pipefd[0]);
    } else { // Parent process
        close(pipefd[0]); // Close read end
        const char* msg = "Hello from parent!";
        write(pipefd[1], msg, strlen(msg) + 1);
        close(pipefd[1]);
    }

    return 0;
}