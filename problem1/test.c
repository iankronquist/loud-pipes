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
void fork_and_run(int read_from_parent[2], int write_to_parent[2],
                  int funcs);

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
    fork_and_run(p1, p2, 0);
}

void fork_and_run(int read_from_parent[2], int write_to_parent[2],
                  int funcs) {
    void (*func_list[])(void) = { sed, tr, awk, sort, uniq };
    int num_funcs = 5;
    if (funcs >= num_funcs) {
        return;
    }
    int read_from_child[2];
    int write_to_child[2];
    if(pipe(read_from_child))
    {
        perror("Error opening pipe to read from child");
        exit(1);
    }
    if(pipe(write_to_child))
    {
        perror("Error opening pipe to write to child");
        exit(1);
    }
    switch(fork())
    {
        case -1:
            perror("fork error");
            exit(0);
        case  0: // child process // 0 is read 1 is write
            close(read_from_parent[1]);
            close(write_to_parent[0]);
            func_list[funcs]();
        default: // parent process
            dup2(write_to_child[0], STDOUT_FILENO);
            dup2(read_from_parent[1], STDIN_FILENO);
            close(read_from_child[1]);
            close(write_to_child[0]);
            fork_and_run(write_to_child, read_from_child, ++funcs);
            break;
    }
}

void uniq() {
    execlp("uniq", "uniq", "-c", NULL); 
}

void cat() {
    execlp("cat", "cat", "file.txt", NULL);
}

void sort() {
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
