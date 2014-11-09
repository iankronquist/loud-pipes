#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
void sort();
void cat();
void tail();
void tr();
void awk();
void uniq();
void sed();
void fork_and_run(int pipes[2], int funcs);

int main()
{
    int p1[2];

    if(pipe(p1))
    {
        perror("pipe1");
        exit(1);
    }

    fork_and_run(p1, 0);
}

void fork_and_run(int pipes[2], int funcs) {
    void (*func_list[])(void) = { sed, tr, awk, sort, uniq };
    int num_funcs = 4;
    wait(NULL);
    if (funcs >= num_funcs) {
        return;
    }
    switch(fork())
    {
        case -1:
            perror("Error forking");
            exit(0);
        case  0: // child process // 0 is read 1 is write
            close(pipes[0]);
            dup2(pipes[1], STDOUT_FILENO);
            close(pipes[1]);
            func_list[funcs]();
        default: // parent process
            close(pipes[1]);
            if (funcs == num_funcs-4)
                dup2(pipes[0], STDIN_FILENO);
            close(pipes[0]);
            fork_and_run(pipes, ++funcs);
            break;
    }
}

void uniq() {
    fprintf(stderr, "uniq");
    execlp("uniq", "uniq", "-c", NULL); 
}

void cat() {
    fprintf(stderr, "cat");
    execlp("cat", "cat", "file.txt", NULL);
}

void sort() {
    fprintf(stderr, "sort");
    execlp("sort", "sort", NULL);
}

void awk() {
    execlp("awk", "awk", "{ for(i = 1; i <= NF; i++) { print $i; } }", NULL);
}

void tr() {
    execlp("tr", "tr", "[A-Z]", "[a-z]", NULL);
}

void sed() {
    execlp("sed", "sed", "s/[^a-zA-Z]/ /g", NULL);
}
