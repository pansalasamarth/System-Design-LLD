#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    int x = 3;
    int pid = fork();

    if(pid == 0)
    {
        x++;
    }

    cout << x << endl;

    if(pid != 0)
    {
        wait(NULL);
    }

    return 0;
}