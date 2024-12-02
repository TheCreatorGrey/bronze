#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// A structure in which variables will be stored
struct VarStorage {
    int integers[64];
    float floats[64];
    int strings[64];
};


// Read and refine script files
char** parse(char* filename) {
    // Read script file
    FILE* fptr;
    fptr = fopen(filename, "r");

    // Will store pointers for each line
    char** lines = NULL;
    int lineCount = 0;

    // This will be toggled if the reader comes
    // across a comment symbol (`)
    bool commentIgnore = false;

    // Whether the parser is on the beginning
    // of a line or not. Tells the reader
    // to ignore following spaces and is disabled
    // when non-space characters are found.
    bool lineBegin = true;
    
    // Start line length capacity at 64
    int lineMemory = 64;
    char* lineBuffer = malloc(lineMemory);
    char ch;
    while (ch != EOF) {
        ch = fgetc(fptr);

        // Ignore newlines
        if (ch == *"\n") {
            continue;
        }
        
        // Toggle commentIgnore if there is a comment symbol
        if (ch == *"`") {
            commentIgnore = !commentIgnore;
            continue;
        }

        // Ignore following chars if commentIgnore is true
        if (commentIgnore) {
            continue;
        }

        // Ignore spaces or indents at line beginnings
        if (ch == *" ") {
            if (lineBegin) {
                continue;
            }
        } else {
            lineBegin = false;
        }


        // Store line on semicolons
        if (ch == *";") {
            // Reallocate to hold another pointer (plus space for sentinel at the end)
            lines = realloc(lines, (lineCount+1)*sizeof(char*));

            // Make copy of line buffer
            char* copy = malloc(strlen(lineBuffer)+1);
            strcpy(copy, lineBuffer);

            // Append pointer of copy array of lines
            lines[lineCount] = copy;

            // Reset line buffer for next line
            lineMemory = 64;
            lineBuffer = realloc(lineBuffer, lineMemory);
            lineBuffer[0] = '\0';
            lineCount++;

            lineBegin = true;
        } else {
            // Append char to line buffer
            int len = strlen(lineBuffer);

            // Allocate more memory for line buffer if the line is too long
            if (lineMemory < len+1) {
                lineMemory += 128;
                lineBuffer = realloc(lineBuffer, lineMemory);
            }

            lineBuffer[len] = ch;
            lineBuffer[len + 1] = '\0';
        }
    }

    // Add sentinel
    char* sentinel = "END";
    lines[lineCount] = sentinel;

    return lines;
}


// Execute internal Bronze command (make, mod, log, etc...)
void executeInternal(char** obj) {

}


int main() {
    char** main = parse("main.bze");

    int currentLine = 0;
    char* line;
    while (true) {
        line = main[currentLine];
        printf("\n%s", line);

        // Break if sentinel (end of script) is found
        if (strcmp(line, "END") == 0) {
            break;
        }

        int lineLength = strlen(line);
        for (int i=0; i<lineLength; i++) {
            char ch = line[i];

            printf("%c\n", ch);
        }

        currentLine++;
    }

    return 0;
}