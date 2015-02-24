#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include "fifo.h"

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

/*int create_pipes(
    int file_des[2];
*/

char *filter_to_lower(char *in_buf, size_t buf_size) {
    char *out_buf = malloc(buf_size);
    for (size_t i = 0; i < buf_size; ++i) {
        out_buf[i] = tolower(in_buf[i]);
    }
    return out_buf;
}

char *filter_sort(char *in_buf, size_t buf_size) {
    char **word_list = malloc(buf_size);
    size_t word_list_index = 0;
    size_t word_counter = 0;
    for (size_t i = 0; i < buf_size; ++i) {
        if (in_buf[i] == '\n') {
            in_buf[i] = '\0';
            word_list[word_list_index] = in_buf[word_counter];
            word_list_index++;
            word_counter = i + 1;
        }
    }
    qsort(word_list, word_list_index, sizeof(char*), strcmp);
    return word_list;
}

void filter_split_lines(char *in_buf, size_t buf_size) {
    for (size_t i = 0; i < buf_size; ++i) {
        if (in_buf[i] == ' ') {
            in_buf[i] = '\n';
        }
    }
    return in_buf;
}

char *filter_alpha_numeric(char *in_buf, size_t *buf_size) {
    char *out_buf = malloc(buf_size);
    size_t j = 0;
    for (size_t i = 0; i < *buf_size; ++i) {
        if (isalnum(in_buf[i]) || in_buf[i] == ' ') {
            out_buf[j] = in_buf[i];
            j++;
        }
    }
    *buf_size = j;
    return out_buf;
}

char *filter_dumb_unique(char *in_words, size_t *buf_size) {
    char *out_buf = malloc(*buf_size * sizeof(char*));
    size_t out_buf_size = 0;
    bool found = false;
    for (size_t i = 0; i < *buf_size; ++i) {
        for (size_t j = 0; j < out_buf_size; ++j) {
            if (strcmp(in_words[i], out_buf[j]) == 0) {
                found = true;
                break;
            }
        }
        if (found) {
            out_buf[out_buf_size] = &buf_size[i];
            out_buf_size++;
            found = false;
        }
    }
    buf_size = out_buf_size;
    return out_buf;
}
