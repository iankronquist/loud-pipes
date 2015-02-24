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
void only_sed();

int main(int argc, char *argv[]) { // read is 0 write is 1

    int in_to_sed[2];
    int sed_to_tr[2];
    int tr_to_awk[2];
    int awk_to_sort[2];
    int sort_to_uniq[2];
    int uniq_out[2];
    if (pipe(in_to_sed) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }
    if (pipe(sed_to_tr) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }
    if (pipe(tr_to_awk) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }    if (pipe(awk_to_sort) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }    if (pipe(sort_to_uniq) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }    if (pipe(uniq_out) == -1) {
        perror("Couldn't opend pipes!");
        exit(-1);
    }
    switch (fork()) {
        case -1:
            perror("Couldn't fork properly");
            exit(-1);
        case 0: //child
            dup2(sed_to_tr[1], STDOUT_FILENO);
            close(sed_to_tr[0]);
            close(sed_to_tr[1]);
            if (argc > 1) {
                execlp("sed", "sed", "s/[^a-zA-Z]/ /g", argv[1], NULL);
            } else {
                execlp("sed", "sed", "s/[^a-zA-Z]/ /g", NULL);
            }
        default:
            close(sed_to_tr[1]);
            break;
    }
    switch(fork()) {
        case -1:
            perror("Couldn't fork properly");
            exit(-1);
        case 0:
            dup2(sed_to_tr[0], STDIN_FILENO);
            dup2(tr_to_awk[1], STDOUT_FILENO);
            close(sed_to_tr[0]);
            close(tr_to_awk[1]);
            execlp("tr", "tr", "[A-Z]", "[a-z]", NULL);
        default:
            close(sed_to_tr[0]);
            close(tr_to_awk[1]);
            break;
    }
    switch (fork()) {
        case -1:
            perror("Couldn't fork properly");
            exit(-1);
        case 0: //child
            dup2(tr_to_awk[0], STDIN_FILENO);
            dup2(awk_to_sort[1], STDOUT_FILENO);
            close(tr_to_awk[0]);
            close(awk_to_sort[1]);
            execlp("awk", "awk", "{ for(i = 1; i <= NF; i++) { print $i; } }", NULL);
        default:
            close(tr_to_awk[0]);
            close(awk_to_sort[1]);
            break;
    }
    switch (fork()) {
        case -1:
            perror("Couldn't fork properly");
            exit(-1);
        case 0: //child
            dup2(awk_to_sort[0], STDIN_FILENO);
            dup2(sort_to_uniq[1], STDOUT_FILENO);
            close(awk_to_sort[0]);
            close(sort_to_uniq[1]);
            execlp("sort", "sort", NULL);
        default:
            close(awk_to_sort[0]);
            close(sort_to_uniq[1]);
            break;
    }
    switch (fork()) {
        case -1:
            perror("Couldn't fork properly");
            exit(-1);
        case 0: //child
            dup2(sort_to_uniq[0], STDIN_FILENO);
            close(sort_to_uniq[0]);
            execlp("uniq", "uniq", "-c", NULL);
        default:
            close(sort_to_uniq[0]);
            break;
    }

    return 0;
}
