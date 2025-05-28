#ifndef S21_GREP_H
#define S21_GREP_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATTERN_SIZE 4096
#define MAX_LINE_LENGTH 4096
#define MAX_PATTERNS 100

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char patterns[MAX_PATTERNS][MAX_PATTERN_SIZE];
  int pattern_count;
  char pattern_file[MAX_PATTERN_SIZE];
} grep_flags_t;

void parse_flags(int argc, char **argv, grep_flags_t *flags);
void read_patterns_from_file(const char *filename, grep_flags_t *flags);
void compile_regex(regex_t *regex, const char *pattern, int cflags);
void combine_patterns(grep_flags_t *flags, char *combined_pattern);
void grep_file(const char *filename, grep_flags_t *flags, int print_filename,
               int *total_match_count);

#endif  // S21_GREP_H