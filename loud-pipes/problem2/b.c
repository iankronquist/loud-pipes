#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

int main() {
    char *fifo_name = "fifo";
    int fifo = open(fifo_name, O_RDWR);
    char buf[] = "hello!\0";
    write(STDERR_FILENO, &buf, 7);
    for(int i = 0; i < 1; i++) {
        write(fifo, &buf, 7);
    }
}
