#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "Common.h"

using namespace std;

int main() {
    // Open shared memory
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    SharedData* shm = (SharedData*)mmap(NULL, sizeof(SharedData),
                                        PROT_READ, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd);

    // Open semaphores
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, 0);
    sem_t* sem_full  = sem_open(SEM_FULL_NAME, 0);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    for (int i = 1; i <= 5; i++) {
        sem_wait(sem_full);   // wait for data

        cout << "Consumed: " << shm->value << endl;

        sem_post(sem_empty);  // signal buffer empty
    }

    munmap(shm, sizeof(SharedData));
    sem_close(sem_empty);
    sem_close(sem_full);

    return 0;
}