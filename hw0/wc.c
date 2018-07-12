#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("hi\n");

    char* fileName = malloc(strlen(argv[0]) + 1); // create space for the string variable
    strcpy(fileName, argv[1]); // copy the characters over to the fileName

    FILE *fp; // create a file object
    fp = fopen(fileName, "r"); // we want to open the file for writing
    if (fp == NULL) {
        printf("File does not exist.\n");
        return 1;
    }

    // initialise all our counters
    int newLineCount = 0;
    int wordCount = 0;
    int characterCount = 0;

    while(1) { // while true (1 in C)
        char c = fgetc(fp);  // use filegetchar (fgetc) to get the next character

        if (feof(fp)) { // if it's the eof character, break
            wordCount++;
            break;
        }

        if (c == ' ') {
            wordCount++;
        }

        if (c == '\n') {
            newLineCount++;
            wordCount++;
        }

        characterCount++;
    }

    fclose(fp);
    printf("  %d  ", newLineCount); // prints the number of characters in the file
    printf(" %d ", wordCount); // prints the number of characters in the file
    printf("%d ", characterCount); // prints the number of characters in the file
    printf("%s\n", fileName); // prints the first argument (file name)
    free(fileName); // releases the memory for the fileName
    return 0;
}

