#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void repeat_function(int s)
{
    cout << "Hello world" << endl; //Place your action here
    alarm(5);
    signal(SIGALRM, repeat_function);
}

int main()
{
    signal(SIGALRM, repeat_function);
    alarm(5);
    while (1)
    {
    }
    return 0;
}
