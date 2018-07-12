#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    int totalNewLineCount = 0;
    int totalWordCount = 0;
    int totalCharacterCount = 0;

    int i;
    for (i = 1; i < argc; i++) {

        char* fileName = malloc(strlen(argv[i]) + 1); // create space for the string variable
        strcpy(fileName, argv[i]); // copy the characters over to the fileName

        FILE *fp; // create a file object
        fp = fopen(fileName, "r"); // we want to open the file for writing
        if (fp == NULL) {
            printf("%s\n", "File does not exist.");
            return 1;
        }

        // initialise all our counters
        int newLineCount = 0;
        int wordCount = 0;
        int characterCount = 0;

        int isWhiteSpace = 1;

        while(1) { // while true (1 in C)
            char c = fgetc(fp);  // use filegetchar (fgetc) to get the next character

            // if it's the end of file character, break the loop
            if (feof(fp)) { // if it's the eof character, break
                //wordCount++;
                break;
            }

            // if it's white space, only increment the number of characters
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { // keep a list of whitespace characters in a set for faster access later
                if (c == '\n') {
                    newLineCount++;
                }
                if (!isWhiteSpace) {
                    // then we have finished a word
                    isWhiteSpace = 1;
                }
            } else { // otherwise it's not whitespace and is considered a word
                if (isWhiteSpace) {
                    // then we are now starting a word
                    wordCount++;
                    isWhiteSpace = 0;
                }
            }

            characterCount++; // increment character count in all cases
        }

        totalNewLineCount += newLineCount;
        totalWordCount += wordCount;
        totalCharacterCount += characterCount;

        fclose(fp);

        printf("%4d", newLineCount); // prints the number of characters in the file
        printf("%5d", wordCount); // prints the number of characters in the file
        printf("%5d ", characterCount); // prints the number of characters in the file
        printf("%s\n", fileName); // prints the first argument (file name)

        free(fileName); // releases the memory for the fileName
    }

    printf("%d", totalNewLineCount); // prints the total number of characters in the file
    printf("%d", totalWordCount); // prints the total number of characters in the file
    printf("%d", totalCharacterCount); // prints the total number of characters in the file
    printf("%s\n", "total"); // prints the first argument (file name)

    return 0;
}

