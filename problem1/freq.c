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
void pipe_and_filter (FILE *in_file, char ** exec_commands[], int num_commands,
                      int cur_command);

int main(int argc, char *argv[]) {
    FILE *in_file;
    if (argc > 1) {
        in_file = fopen(argv[1], "r");
    } else {
        in_file = stdin;
    }
    char** commands[] = {
        {"true", "\0"},
        {"tr", "'[A-Z]'", "'[a-z]'", "\0"},
        {"sed", "'s/[^a-zA-Z]/ /g'", "\0"}
    };
    pipe_and_filter(in_file, commands, 3, 0);
    return 0;
}

void pipe_and_filter (FILE *in_file, char ** exec_commands[], int num_commands,
                      int cur_command) {
    pid_t pid;
    int pipes_to_children[2]; // 0 is read 1 is write
    int pipes_from_children[2]; // 0 is read 1 is write
    if (pipe(pipes_to_children) != 0 && pipe(pipes_from_children) != 0) {
        printf("%s\n", errno == EFAULT ? "EFAULT" : "Not EFAULT");
        printf("%s\n", errno == EMFILE ? "EMFILE" : "Not EMFILE");
        assert(0);
    }
    pid = fork();
    fprintf(stderr, "Fork pid %d\n", pid);
    if (pid == (pid_t)NULL) { // if this is the child process
        dup2(pipes_to_children[0], STDIN_FILENO);
        dup2(pipes_from_children[1], STDOUT_FILENO);
        close(pipes_to_children[1]);
        close(pipes_to_children[0]);
        close(pipes_from_children[1]);
        close(pipes_from_children[0]);
        fprintf(stderr, "Running %s %d\n", exec_commands[cur_command], cur_command);
        if (cur_command <  num_commands)
        {
            pipe_and_filter(pipes_to_children[0], exec_commands, num_commands, ++cur_command);
            execvp(exec_commands[cur_command][0], &exec_commands[cur_command][1]);
        }
    } else { // else this is the parent process
        FILE *from_ch_stream = fdopen(pipes_from_children[0], "r");
        FILE *to_ch_stream = fdopen(pipes_to_children[1], "w");
        if (cur_command < num_commands) {
            while (!feof(from_ch_stream)) {
               fputc(fgetc(from_ch_stream), to_ch_stream);
               fputc(fgetc(from_ch_stream), stderr);
            }
        }
        fclose(from_ch_stream);
        fclose(to_ch_stream);
        close(pipes_to_children[1]);
        close(pipes_to_children[0]);
        close(pipes_from_children[1]);
        close(pipes_from_children[0]);
        kill(pid, SIGTERM);
    }
    fprintf(stderr, "Exit pid %d\n", pid);
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
