#ifndef COMMON_H
#define COMMON_H

#define SHM_NAME "/myshm_example"
#define SEM_EMPTY_NAME "/mysem_empty"
#define SEM_FULL_NAME  "/mysem_full"

struct SharedData {
    int value;
};

#endif