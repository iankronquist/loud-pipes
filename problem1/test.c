#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
void sort(int p1[2], int p2[2]);
void cat(int p1[2], int p2[2]);
void tail(int p1[2], int p2[2]);
void tr(int p1[2], int p2[2]);
void awk(int p1[2], int p2[2]);
void uniq(int p1[2], int p2[2]);
void sed(int p1[2], int p2[2]);
void fork_and_run(int p1[2], int p2[2], void (*func)(int p1[2], int p2[2]));

int main()
{
    int p1[2];
    int p2[2];

    if(pipe(p1))
    {
        perror("pipe1");
        exit(1);
    }

    if(pipe(p2))
    {
        perror("pipe2");
        exit(1);
    }
    fork_and_run(p1, p2, cat);
}

void fork_and_run(int p1[2], int p2[2], void (*func)(int p1[2], int p2[2])) {
    switch(fork())
    {
        case -1:
            perror("fork error");
            exit(0);
        case  0: // child process
            func(p1, p2);
        default: // parent process
            break;
    }
}

//last
void uniq(int p1[2], int p2[2]) {
    dup2(p2[0], STDIN_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p1[0]);
    close(p1[1]);
    execlp("uniq", "uniq", "-c", NULL); 
}

//first
void cat(int p1[2], int p2[2]) {
    fork_and_run(p1, p2, sed);
    dup2(p1[1], STDOUT_FILENO);
    close(p1[1]);
    close(p1[0]);
    execlp("cat", "cat", "file.txt", NULL);
}

//mid
void sort(int p1[2], int p2[2]) {
    fork_and_run(p1, p2, uniq);
    dup2(p1[0], STDIN_FILENO);
    dup2(p2[1], STDOUT_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p1[0]);
    close(p1[1]);
    execlp("sort", "sort", NULL);
}

void awk(int p1[2], int p2[2]) {
    fork_and_run(p1, p2, sort);
    dup2(p1[0], STDIN_FILENO);
    dup2(p2[1], STDOUT_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p1[0]);
    close(p1[1]);
    execlp("awk", "awk", "{ for(i = 1; i <= NF; i++) { print $i; } }", NULL);
}

void tr(int p1[2], int p2[2]) {
    fork_and_run(p1, p2, awk);
    dup2(p1[0], STDIN_FILENO);
    dup2(p2[1], STDOUT_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p1[0]);
    close(p1[1]);
    execlp("tr", "tr", "[A-Z]", "[a-z]", NULL);
}

void sed(int p1[2], int p2[2]) {
    fork_and_run(p1, p2, tr);
    dup2(p1[0], STDIN_FILENO);
    dup2(p2[1], STDOUT_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p1[0]);
    close(p1[1]);
    execlp("sed", "sed", "s/[^a-zA-Z]/ /g", NULL);
}
