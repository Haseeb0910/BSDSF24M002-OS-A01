#include <stdio.h>
#include <string.h>
#include "mystrfunctions.h"
#include "myfilefunctions.h"

int main() {
    char str1[] = "Hello World";
    char str2[] = "  trim me please  ";
    char str3[] = "madam";
    char str4[] = "hello";

    printf("Original: %s\n", str1);

    str_reverse(str1);
    printf("Reversed: %s\n", str1);

    str_to_upper(str1);
    printf("Uppercase: %s\n", str1);

    str_to_lower(str1);
    printf("Lowercase: %s\n", str1);

    printf("Before trim: '%s'\n", str2);
    str_trim(str2);
    printf("After trim: '%s'\n", str2);

    printf("Count of 'l' in \"%s\": %d\n", str4, str_count_char(str4, 'l'));

    printf("Is \"%s\" a palindrome? %s\n", str3, str_is_palindrome(str3) ? "Yes" : "No");
    printf("Is \"%s\" a palindrome? %s\n", str4, str_is_palindrome(str4) ? "Yes" : "No");

    printf("\n--- File Functions ---\n");

    const char *testfile = "sample.txt";

    int words = file_word_count(testfile);
    printf("Word count in %s: %d\n", testfile, words);

    int lines = file_line_count(testfile);
    printf("Line count in %s: %d\n", testfile, lines);

    printf("Searching for pattern \"hello\":\n");
    int matches = file_search_pattern(testfile, "hello");
    printf("Total matches: %d\n", matches);

    return 0;
}
