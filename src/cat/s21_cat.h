#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdbool.h>
#include <stdio.h>

void print_usage(void);
void handle_non_newline(char c, bool show_tabs, bool show_nonprinting);
void handle_newline(bool show_ends);
void print_file(FILE *file, bool number_lines, bool number_nonblank_lines,
                bool show_ends, bool squeeze_blank_lines, bool show_tabs,
                bool show_nonprinting);
int process_arguments(int argc, char *argv[], bool *number_lines,
                      bool *number_nonblank_lines, bool *show_ends,
                      bool *squeeze_blank_lines, bool *show_tabs,
                      bool *show_nonprinting);

#endif  // CAT_H