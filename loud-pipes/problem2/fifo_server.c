#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "fifo.h"
void die(int sig);
void create_fifo(char *fifo_path);
void read_loop(int fifo);
void fork_and_serve(pid_t master);
void read_commands(pid_t master);
void fifo_read_loop(FILE *fifo);

int main() {
    signal(SIGINT, die);
    char fifo_path[500];
    CREATE_SERVER_FIFO_NAME(fifo_path);
    fprintf(stderr, "%s\n", fifo_path);
    remove(fifo_path);
    int err = mkfifo(fifo_path, FIFO_PERMISSIONS);
    if (err) perror("Could not create fifo");
    int fifo = open(fifo_path, O_RDWR);
    int dummy_fifo = open(fifo_path, O_RDWR);
    pid_t client_pid;
    read_loop(fifo);
    wait(0);

    return 0;
}

void die(int sig) {
    fprintf(stderr, "asdf");
    // TODO: kill children
    char fifo_path[500];
    CREATE_SERVER_FIFO_NAME(fifo_path);
    remove(fifo_path);
    exit(0);
}


void read_loop(int fifo) {
    fprintf(stderr, "parent server read loop\n");
    pid_t child;
    while(read(fifo, &child, sizeof(pid_t)) > 0) {
        fprintf(stderr, "parent server received |%d|\n", child);
        fork_and_serve(child);
    }
    perror("Error reading from server fifo");
    assert(0); //not reached
}

void fork_and_serve(pid_t master) {
    fprintf(stderr, "Serving: %d\n", master);
    pid_t cur = fork();
    if (cur == 0) {
        return;
    } else if (cur == -1) {
        perror("Could not fork!");
    } else { // the server returns to the read loop
        read_commands(master);
    }
}

void read_commands(pid_t master) {
    fprintf(stderr, "created a child-server");
    int read_master, write_master;
    char name_buffer[500];
    // TODO: don't reuse buffers
    CREATE_CLIENT_WRITER_NAME(name_buffer, master);
    fprintf(stderr, "Child server is reading from %s\n", name_buffer);
    read_master = open(name_buffer, O_RDWR);
    CREATE_CLIENT_READER_NAME(name_buffer, master);
    fprintf(stderr, "Child server is writing to %s\n", name_buffer);
    write_master = open(name_buffer, O_RDWR);
    int message_size = 0;
    char *command;
    while(read(read_master, &message_size, sizeof(int)) > 0) {
        if (message_size == 0) continue;
        command = malloc(message_size+1);
        assert(command != NULL);
        command[message_size] = '\0';
        read(read_master, command, message_size);
        fprintf(stderr, "parent child server received len|%d|\n", message_size);
        fprintf(stderr, "parent child server received |%s|\n", command);
        if (strstr(command, "exit") != 0) {
            fprintf(stderr, "exit");
            int s = 7;
            write(write_master, &s, sizeof(int));
            write(write_master, "exited", 7);
            break;
        } else if (strstr(command, "cd") != 0) {
            fprintf(stderr, "cd");
            int s = 5;
            write(write_master, &s, sizeof(int));
            write(write_master, "cded", 5);
        } else if (strstr(command, "dir") != 0) {
            fprintf(stderr, "dir");
            int s = 5;
            write(write_master, &s, sizeof(int));
            write(write_master, "dired", 5);

        } else if (strstr(command, "pwd") != 0) {
            fprintf(stderr, "pwd");
            int s = 6;
            write(write_master, &s, sizeof(int));
            write(write_master, "pwded", 6);
        } else {
            int s = 17;
            write(write_master, &s, sizeof(int));
            write(write_master, "Invalid command!", 17);
        }
        free(command);
    }
}

