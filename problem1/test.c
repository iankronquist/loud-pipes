#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void sed(void);
void tr(void);
void fork_and_run(void (*func)(void), int pipe_back);

int main(int argc, char **argv) {
    fork_and_run(sed, 0);
    return 0;
}

void fork_and_run(void (*func)(void), int pipe_back) {
    pid_t pid;
    int pipes[2]; // 0 is read 1 is write
    switch (pid = fork()) {
        case 0: // child
            if (pipe_back) {
                if (pipe(pipes)) {
                    perror("The pipes are clogged!");
                    exit(1);
                }
                dup2(pipes[1], STDOUT_FILENO);
                dup2(pipes[0], STDIN_FILENO);
            }
            func();
            break;
        default: // parent
            break;
        case -1: // fork failed
            perror("Could not fork!");
            exit(1);
    }
}

void sed(void) {
    fputs("sed\n", stderr);
    fork_and_run(tr, 0);
    execlp("sed", "sed", "s/[^a-zA-Z]/ /g", (char*)0);
    perror("Error running sed!");
    exit(1);
}

void tr(void) {
    fputs("tr\n", stderr);
    execlp("tr", "tr", "[A-Z]", "[a-z]",(char*)0);
    perror("Error running tr!");
    exit(1);
}
