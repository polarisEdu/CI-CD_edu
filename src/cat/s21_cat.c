#include "s21_cat.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
  printf(
      "Usage: cat [OPTION]... [FILE]...\n"
      "Concatenate FILE(s) to standard output.\n\n"
      "With no FILE, or when FILE is -, read standard input.\n\n"
      "  -b, --number-nonblank     number nonempty output lines\n"
      "  -e                        equivalent to -vE, but leaves tabs "
      "unchanged\n"
      "  -E                        display $ at end of each line\n"
      "  -n, --number              number all output lines\n"
      "  -s, --squeeze-blank       suppress repeated empty output lines\n"
      "  -t                        equivalent to -vT\n"
      "  -T                        display TAB characters as ^I\n"
      "  -v                        use ^ and M- notation, except for LFD and "
      "TAB\n");
}

void handle_non_newline(char c, bool show_tabs, bool show_nonprinting) {
  if (show_tabs && c == '\t') {
    printf("^I");
  } else if (show_nonprinting && c == 127) {
    printf("^?");
  } else if (show_nonprinting && !isprint(c) && c != '\n' && c != '\t') {
    printf("^%c", c + 64);
  } else {
    putchar(c);
  }
}

void handle_newline(bool show_ends) {
  if (show_ends) {
    printf("$");
  }
  putchar('\n');
}

void print_file(FILE *file, bool number_lines, bool number_nonblank_lines,
                bool show_ends, bool squeeze_blank_lines, bool show_tabs,
                bool show_nonprinting) {
  int c;
  int line_number = 1;
  int newline_count = 0;
  bool new_line = true;

  while ((c = fgetc(file)) != EOF) {
    if (new_line) {
      if (number_lines || (number_nonblank_lines && c != '\n')) {
        printf("%6d\t", line_number++);
      }
      new_line = false;
    }

    if (c == '\n') {
      newline_count++;
      if (squeeze_blank_lines && newline_count > 2) {
        continue;
      }
      handle_newline(show_ends);
      new_line = true;
    } else {
      newline_count = 0;
      handle_non_newline(c, show_tabs, show_nonprinting);
    }
  }
}

int process_arguments(int argc, char *argv[], bool *number_lines,
                      bool *number_nonblank_lines, bool *show_ends,
                      bool *squeeze_blank_lines, bool *show_tabs,
                      bool *show_nonprinting) {
  bool files_started = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--") == 0) {
      files_started = true;
      continue;
    }

    if (files_started || argv[i][0] != '-') {
      FILE *file = fopen(argv[i], "r");
      if (file == NULL) {
        perror(argv[i]);
        continue;
      }
      print_file(file, *number_lines, *number_nonblank_lines, *show_ends,
                 *squeeze_blank_lines, *show_tabs, *show_nonprinting);
      fclose(file);
    } else {
      for (int j = 1; argv[i][j] != '\0'; j++) {
        switch (argv[i][j]) {
          case 'b':
            *number_nonblank_lines = true;
            break;
          case 'e':
            *show_nonprinting = true;
            *show_ends = true;
            break;
          case 'E':
            *show_ends = true;
            break;
          case 'n':
            *number_lines = true;
            break;
          case 's':
            *squeeze_blank_lines = true;
            break;
          case 't':
            *show_nonprinting = true;
            *show_tabs = true;
            break;
          case 'T':
            *show_tabs = true;
            break;
          case 'v':
            *show_nonprinting = true;
            break;
          default:
            fprintf(stderr, "Unknown option: -%c\n", argv[i][j]);
            return EXIT_FAILURE;
        }
      }
    }
  }
  return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    print_usage();
    return EXIT_FAILURE;
  }

  bool number_lines = false;
  bool number_nonblank_lines = false;
  bool show_ends = false;
  bool squeeze_blank_lines = false;
  bool show_tabs = false;
  bool show_nonprinting = false;

  int result = process_arguments(
      argc, argv, &number_lines, &number_nonblank_lines, &show_ends,
      &squeeze_blank_lines, &show_tabs, &show_nonprinting);

  return result;
}