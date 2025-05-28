#include "s21_grep.h"

#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to parse command line flags and options
void parse_flags(int argc, char **argv, grep_flags_t *flags) {
  int opt;
  int option_index = 0;

  // Define long options for command line parsing
  static struct option long_options[] = {
      {"pattern", required_argument, 0, 'e'},
      {"ignore-case", no_argument, 0, 'i'},
      {"invert-match", no_argument, 0, 'v'},
      {"count", no_argument, 0, 'c'},
      {"files-with-matches", no_argument, 0, 'l'},
      {"line-number", no_argument, 0, 'n'},
      {"no-filename", no_argument, 0, 'h'},
      {"silent", no_argument, 0, 's'},
      {"file", required_argument, 0, 'f'},
      {"only-matching", no_argument, 0, 'o'},
      {0, 0, 0, 0}};

  // Parse command line options
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        // Copy pattern to flags structure
        strcpy(flags->patterns[flags->pattern_count++], optarg);
        break;
      case 'i':
        flags->i = 1;  // Set ignore-case flag
        break;
      case 'v':
        flags->v = 1;  // Set invert-match flag
        break;
      case 'c':
        flags->c = 1;  // Set count flag
        break;
      case 'l':
        flags->l = 1;  // Set files-with-matches flag
        break;
      case 'n':
        flags->n = 1;  // Set line-number flag
        break;
      case 'h':
        flags->h = 1;  // Set no-filename flag
        break;
      case 's':
        flags->s = 1;  // Set silent flag
        break;
      case 'f':
        flags->f = 1;
        // Copy pattern file name to flags structure
        strcpy(flags->pattern_file, optarg);
        break;
      case 'o':
        flags->o = 1;  // Set only-matching flag
        break;
      default:
        // Print usage message to stderr and exit
        fprintf(stderr,
                "Usage: %s [-e pattern] [-i] [-v] [-c] [-l] [-n] [-h] [-s] [-f "
                "file] [-o] [pattern] [file...]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
  }
}

// Function to read patterns from a file
void read_patterns_from_file(const char *filename, grep_flags_t *flags) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    // Handle file open error, respect silent flag
    if (!flags->s) {
      fprintf(stderr, "Error opening pattern file: %s\n", filename);
      perror("fopen");
    }
    exit(EXIT_FAILURE);
  }

  char line[MAX_PATTERN_SIZE];
  // Read patterns from file line by line
  while (fgets(line, sizeof(line), file) &&
         flags->pattern_count < MAX_PATTERNS) {
    strtok(line, "\n");  // Remove newline character
    strcpy(flags->patterns[flags->pattern_count++], line);
  }
  fclose(file);  // Close the file
}

// Function to compile a regular expression
void compile_regex(regex_t *regex, const char *pattern, int cflags) {
  int ret = regcomp(regex, pattern, cflags);
  if (ret) {
    // Handle regex compilation error
    char errbuf[256];
    regerror(ret, regex, errbuf, sizeof(errbuf));
    fprintf(stderr, "Could not compile regex: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }
}

// Function to combine multiple patterns into a single pattern
void combine_patterns(grep_flags_t *flags, char *combined_pattern) {
  if (flags->pattern_count == 1) {
    // Single pattern case
    strcpy(combined_pattern, flags->patterns[0]);
  } else {
    // Multiple patterns case, combine them with alternation
    strcpy(combined_pattern, "(");
    for (int i = 0; i < flags->pattern_count; i++) {
      if (i > 0) strcat(combined_pattern, ")|(");
      strcat(combined_pattern, flags->patterns[i]);
    }
    strcat(combined_pattern, ")");
  }
}

// Function to grep a file based on provided patterns and flags
void grep_file(const char *filename, grep_flags_t *flags, int print_filename,
               int *total_match_count) {
  FILE *file =
      filename ? fopen(filename, "r") : stdin;  // Open file or use stdin
  if (!file) {
    // Handle file open error, respect silent flag
    if (!flags->s) {
      fprintf(stderr, "Error opening input file: %s\n", filename);
      perror("fopen");
    }
    return;
  }

  regex_t regex;
  int cflags = REG_EXTENDED | (flags->i ? REG_ICASE : 0);  // Set regex flags

  char combined_pattern[MAX_PATTERN_SIZE * MAX_PATTERNS];
  combine_patterns(flags, combined_pattern);        // Combine patterns
  compile_regex(&regex, combined_pattern, cflags);  // Compile combined regex

  char line[MAX_LINE_LENGTH];
  int line_num = 0;
  int match_count = 0;
  int any_match = 0;

  // Read lines from file and apply regex
  while (fgets(line, sizeof(line), file)) {
    line_num++;
    int match = !regexec(&regex, line, 0, NULL, 0);  // Check for match

    if (flags->v) match = !match;  // Invert match if needed

    if (match) {
      any_match = 1;
      match_count++;
      if (!flags->c && !flags->l) {
        if (flags->o && !flags->v) {
          regmatch_t pmatch[1];
          const char *p = line;
          // Print only matching parts of the line
          while (!regexec(&regex, p, 1, pmatch, 0)) {
            if (pmatch[0].rm_so != pmatch[0].rm_eo) {
              if (print_filename && !flags->h) printf("%s:", filename);
              if (flags->n) printf("%d:", line_num);
              printf("%.*s\n", (int)(pmatch[0].rm_eo - pmatch[0].rm_so),
                     p + pmatch[0].rm_so);
            }
            p += pmatch[0].rm_eo;
          }
        } else {
          // Print full line with optional filename and line number
          if (print_filename && !flags->h) printf("%s:", filename);
          if (flags->n) printf("%d:", line_num);
          printf("%s", line);
          if (line[strlen(line) - 1] != '\n') printf("\n");
        }
      }
    }
  }

  if (flags->c) {
    // Handle count flag
    *total_match_count += any_match;
    if (flags->l) {
      if (any_match) {
        if (print_filename && !flags->h) printf("%s:", filename);
        printf("%d\n", 1);
      }
    } else {
      if (print_filename && !flags->h) printf("%s:", filename);
      printf("%d\n", match_count);
    }
  }

  if (flags->l && any_match) printf("%s\n", filename);

  regfree(&regex);             // Free regex resources
  if (filename) fclose(file);  // Close file if it was opened
}

int main(int argc, char **argv) {
  grep_flags_t flags = {0};  // Initialize flags structure

  parse_flags(argc, argv, &flags);  // Parse command line flags

  if (flags.f)
    read_patterns_from_file(flags.pattern_file,
                            &flags);  // Read patterns from file if specified

  if (flags.pattern_count == 0 && optind < argc) {
    // No patterns from options, use positional argument
    strcpy(flags.patterns[flags.pattern_count++], argv[optind]);
    optind++;
  }

  if (flags.pattern_count == 0) {
    // No patterns provided, print error and exit
    fprintf(stderr, "No pattern provided\n");
    exit(EXIT_FAILURE);
  }

  int num_files = argc - optind;  // Number of files to process
  int total_match_count = 0;      // Initialize total match count

  if (num_files == 0) {
    // No files provided, read from stdin
    grep_file(NULL, &flags, 0, &total_match_count);
  } else {
    // Process each file provided in command line arguments
    for (int i = optind; i < argc; i++) {
      grep_file(argv[i], &flags, num_files > 1, &total_match_count);
    }
  }

  return 0;
}
