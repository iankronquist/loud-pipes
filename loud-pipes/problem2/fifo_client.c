#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "fifo.h"

void die(int sig);
void read_write_loop(int my_read_fifo, int my_write_fifo);

int main() {
    signal(SIGINT, die);
    pid_t my_pid = getpid();
    char server_fifo_name[500];
    char my_read_fifo_name[500]; // server's write end
    char my_write_fifo_name[500]; // server's read end
    CREATE_SERVER_FIFO_NAME(server_fifo_name);
    fprintf(stderr, "%s\n", server_fifo_name);
    CREATE_CLIENT_READER_NAME(my_read_fifo_name, my_pid);
    CREATE_CLIENT_WRITER_NAME(my_write_fifo_name, my_pid);
    fprintf(stderr, "Client is writing to %s\n", my_write_fifo_name);
    fprintf(stderr, "Client is reading from %s\n", my_read_fifo_name);
    int server_pipe = open(server_fifo_name, O_RDWR);
    write(server_pipe, &my_pid, sizeof(pid_t));
    close(server_pipe);
    int err = mkfifo(my_read_fifo_name, FIFO_PERMISSIONS);
    if (err) perror("Could not create fifo");
    err = mkfifo(my_write_fifo_name, FIFO_PERMISSIONS);
    if (err) perror("Could not create fifo");
    int my_read_fifo = open(my_read_fifo_name, O_RDWR);
    int my_write_fifo = open(my_write_fifo_name, O_RDWR);
    read_write_loop(my_read_fifo, my_write_fifo);
    close(my_read_fifo);
    close(my_write_fifo);
    remove(my_read_fifo_name);
    remove(my_write_fifo_name);
}

void die(int sig) {
    // TODO: can this be done better?
    pid_t my_pid = getpid();
    char my_read_fifo_name[500]; // server's write end
    char my_write_fifo_name[500]; // server's read end
    CREATE_CLIENT_READER_NAME(my_read_fifo_name, my_pid);
    CREATE_CLIENT_WRITER_NAME(my_write_fifo_name, my_pid);
    int my_write_fifo = open(my_write_fifo_name, O_RDWR);
    int exit_len = 5;
    write(my_write_fifo, &exit_len, sizeof(int));
    write(my_write_fifo, "exit", exit_len);
    close(my_write_fifo);
    remove(my_read_fifo_name);
    remove(my_write_fifo_name);
    exit(0);
}

void read_write_loop(int my_read_fifo, int my_write_fifo) {
    char *message;
    char *line = NULL;
    char *in_line = NULL;
    size_t line_cap = 0;
    int line_len;
    int message_size;
    printf("%s", CLIENT_PROMPT);
    while ((line_len = (int)getline(&line, &line_cap, stdin)) > 0) {
        if (strstr(line, "lcd") != 0) {
            fprintf(stderr, "lcd");
        } else if (strstr(line, "ldir") != 0) {
            fprintf(stderr, "ldir");
        } else if (strstr(line, "lpwd") != 0) {
            fprintf(stderr, "lpwd");
        }
        write(my_write_fifo, &line_len, sizeof(int));
        write(my_write_fifo, line, MESSAGE_SIZE);
        if (strstr(line, "exit") != 0) {
            break;
        }
        read(my_read_fifo, &message_size, sizeof(int));
        message = malloc(message_size+1);
        message[message_size] = '\0';
        read(my_read_fifo, message, message_size);
        write(STDOUT_FILENO, message, message_size);
        free(message);
        printf("\n%s", CLIENT_PROMPT);
    }
}
