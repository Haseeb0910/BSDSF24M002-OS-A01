#include <stdio.h>
#include <string.h>
#include "myfilefunctions.h"

// Counts the number of whitespace-separated words in a file
int file_word_count(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    int in_word = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            in_word = 0;
        } else if (in_word == 0) {
            in_word = 1;
            count++;
        }
    }

    fclose(fp);
    return count;
}

// Counts the number of lines in a file (counts '\n' characters)
int file_line_count(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    int count = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }

    fclose(fp);
    return count;
}

// Searches for a pattern (substring) in each line of a file
// Prints matching lines and returns the count of matches
int file_search_pattern(const char *filename, const char *pattern) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    char line[1024];
    int match_count = 0;
    int line_number = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        line_number++;
        if (strstr(line, pattern) != NULL) {
            printf("Line %d: %s", line_number, line);
            match_count++;
        }
    }

    fclose(fp);
    return match_count;
}
