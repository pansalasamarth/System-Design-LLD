#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "Common.h"

using namespace std;

int main() {
    // Create shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    SharedData* shm = (SharedData*)mmap(NULL, sizeof(SharedData),
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    close(fd);

    // Create semaphores
    sem_t* sem_empty = sem_open(SEM_EMPTY_NAME, O_CREAT, 0666, 1);
    sem_t* sem_full  = sem_open(SEM_FULL_NAME,  O_CREAT, 0666, 0);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    for (int i = 1; i <= 5; i++) {
        sleep(2);

        sem_wait(sem_empty);  // wait until buffer empty

        shm->value = i;
        cout << "Produced: " << i << endl;

        sem_post(sem_full);   // signal data available
        sleep(1);
    }

    munmap(shm, sizeof(SharedData));
    sem_close(sem_empty);
    sem_close(sem_full);

    return 0;
}