#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "mystrfunctions.h"

// Reverses a string in place using two pointers moving toward each other
void str_reverse(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Converts every character to uppercase in place
void str_to_upper(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Converts every character to lowercase in place
void str_to_lower(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Removes leading and trailing whitespace in place
void str_trim(char *str) {
    int start = 0;
    int len = strlen(str);
    int end = len - 1;

    // Find first non-whitespace character
    while (isspace((unsigned char)str[start])) {
        start++;
    }

    // Find last non-whitespace character
    while (end > start && isspace((unsigned char)str[end])) {
        end--;
    }

    // Shift the trimmed content to the beginning
    int j = 0;
    for (int i = start; i <= end; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';  // Null-terminate the trimmed string
}

// Counts how many times character c appears in str
int str_count_char(const char *str, char c) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == c) {
            count++;
        }
    }
    return count;
}

// Checks whether str reads the same forwards and backwards
int str_is_palindrome(const char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    while (start < end) {
        if (str[start] != str[end]) {
            return 0;  // Not a palindrome
        }
        start++;
        end--;
    }
    return 1;  // Is a palindrome
}
