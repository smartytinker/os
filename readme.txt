#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    printf("Parent process started (PID: %d)\n", getpid());

    pid = fork();  // create child process

    if (pid < 0) {
        printf("Fork failed!\n");
        return 1;
    }
    else if (pid == 0) {
        // Child process
        printf("Child process running (PID: %d, Parent PID: %d)\n", getpid(), getppid());
        sleep(2); // simulate some work
        printf("Child process finished.\n");
    }
    else {
        // Parent process
        printf("Parent waiting for child to finish...\n");
        wait(NULL); // wait for child process to complete
        printf("Child completed. Parent resumes (PID: %d)\n", getpid());
    }

    return 0;
}

