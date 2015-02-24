#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    size_t buf_size = 1024;
    char *buf = malloc(sizeof(buf_size));
    int nw = 0;
    pid_t pid = fork();
    puts("fork");
    while ((nw = read(STDIN_FILENO, buf, buf_size)) > 0) {
        printf("***%i***\n", nw);
        write(STDOUT_FILENO, buf, buf_size);
    }
}



