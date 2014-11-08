#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
void filter_alpha_numeric(FILE *pipe_in, FILE *pipe_out);
void filter_to_lower(FILE *pipe_in, FILE *pipe_out);
void filter_split(FILE *pipe_in, FILE *pipe_out);
void nop(FILE *pipe_in, FILE *pipe_out);
void pipe_and_filter (FILE *pipe_in, FILE *pipe_out, int count,
    const int num_filters, const void (**filter)(FILE*, FILE*));

int main(int argc, char *argv[]) {
    void (*filter_arr[])(FILE*, FILE*) = {nop, filter_alpha_numeric, filter_to_lower, filter_split};
    int max = 4;
    FILE *in_file;
    if (argc > 1) {
        in_file = fopen(argv[1], "r");
    } else {
        in_file = stdin;
    }
    fprintf(stdout, "test %p\n", stdout);
    pipe_and_filter(in_file, stdout, 0, max, filter_arr);
    return 0;
}

void pipe_and_filter (FILE *pipe_in, FILE *pipe_out, int count,
    const int num_filters, const void (**filter)(FILE*, FILE*)) {
    printf("Process: %d, pipes in %p out %p\n", count, pipe_in, pipe_out);
    pid_t pid;
    int pipes_to_children[2]; // 0 is read 1 is write
    FILE *child_pipe_read, *child_pipe_write;
    if (pipe(pipes_to_children) != 0) {
        printf("%s\n", errno == EFAULT ? "EFAULT" : "Not EFAULT");
        printf("%s\n", errno == EMFILE ? "EMFILE" : "Not EMFILE");
        assert(0);
    }
    child_pipe_read = fdopen(pipes_to_children[0], "r");
    child_pipe_write = fdopen(pipes_to_children[1], "w");
    printf("Process: %d, pipes chin %p chout %p\n", count, child_pipe_read, child_pipe_write);
    pid = fork();
    if (pid == (pid_t)NULL) { // if this is the child process
        fclose(child_pipe_write);
        close(pipes_to_children[1]);
        if (count < num_filters)
        {
            count++;
            pipe_and_filter(child_pipe_read, child_pipe_write,
                count, num_filters, filter);
        }
        fclose(child_pipe_read);
        close(pipes_to_children[0]);
    } else { // else this is the parent process
        fclose(child_pipe_read);
        close(pipes_to_children[0]);
        filter[count](child_pipe_write, child_pipe_read);
        while (!feof(pipe_in)) {
            printf("pid %d put %d\n", pid, count);
            char c = fgetc(pipe_in);
            printf("!!Process: %d, pipes in %p out %p, %c\n", count, pipe_in, pipe_out, c);
            fputc(c, child_pipe_write);
        }
        /*while (!feof(child_pipe_read)) {
            char c = fgetc(child_pipe_read);
            printf("!!Process: %d, pipes in %p out %p, %c\n", count, pipe_in, pipe_out, c);
            fputc(c, pipe_out);
        }*/
        kill(pid, SIGTERM);
        close(pipes_to_children[1]);
        filter[count](child_pipe_read, pipe_out);
    }
    printf("Process %d\n", count);
    printf("Exit pid %d\n", pid);
    exit(0);
}

void nop(FILE *pipe_in, FILE *pipe_out){ puts("nop");}

void filter_alpha_numeric(FILE *pipe_in, FILE *pipe_out) {
    char c;
    while(!feof(pipe_in)) {
        c = fgetc(pipe_in);
        if (isalnum(c)) {
            fputc(c, pipe_out);
            printf("%c", c);
        }
    }
}

void filter_to_lower(FILE *pipe_in, FILE *pipe_out) {
    char c;
    while(!feof(pipe_in)) {
        c = fgetc(pipe_in);
        printf("%c", c);
        fputc(tolower(c), pipe_out);
    }
}

void filter_split(FILE *pipe_in, FILE *pipe_out) {
    char c;
    while(!feof(pipe_in)) {
        c = fgetc(pipe_in);
        printf("%c", c);
        if (c == ' ') {
            fputc('\n', pipe_out);
        }
    }
}
