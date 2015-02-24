#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define FIFO_PERMISSIONS  S_IRUSR | S_IWUSR

int main() {
    char *fifo_name = "fifo";
    mkfifo(fifo_name, FIFO_PERMISSIONS);
    int fifo = open(fifo_name, O_RDWR);
    char buf[7];
    while (read(fifo, &buf, 7) >= 0) {
        write(STDERR_FILENO, &buf, 7);
    }
}
