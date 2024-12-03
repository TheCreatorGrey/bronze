#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*\

BRONZE GLOSSARY

Script:
A file or chunk of text which holds bronze code.

Lines:
Commands separated by semicolons (;)
Not always the same as the lines in the file.

Segment:
Parts of a command separated by spaces

*/

// A structure in which variables will be stored
struct VarStorage {
    int integers[64];
    float floats[64];
    int strings[64];
};

char* lnsentinel = "LINE_END";

// Read and refine script files
char*** parse(char* filename) {
    // Read script file
    FILE* fptr;
    fptr = fopen(filename, "r");

    // Will store pointers for each line
    char*** lines = NULL; // Where's my award?
    int lineCount = 0;

    // This will be toggled if the reader comes
    // across a comment symbol (`)
    bool commentIgnore = false;

    // This will be toggled if the reader comes
    // across parentheses, telling it to ignore spaces in-between them.
    bool ignoreSpaces = false;

    // Whether the parser is on the beginning
    // of a line or not. Tells the reader
    // to ignore following spaces and is disabled
    // when non-space characters are found.
    bool lineBegin = true;

    bool lineHasData = false;
    
    int segmentIndex = 0; // Index of current segment of current line being written to
    int segmentMemory = 8; // Default memory given to a segment
    char* segmentBuffer = malloc(segmentMemory);

    char** lineBuffer = NULL; //malloc(lineMemory);
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

        // Enable/disable ignoreSpaces if there are parentheses
        if (ch == *"(") {
            ignoreSpaces = true;
            continue;
        } if (ch == *")") {
            ignoreSpaces = false;
            continue;
        }

        if (!ignoreSpaces) {
            if (ch == *" ") {
                // Ignore spaces or indents at line beginnings
                if (lineBegin) {
                    continue;
                }

                // If there is a space after a line begin,
                // it (usually) indicates the end of an argument or segment of a line.

                // Allocate space in line for another segment pointer
                lineBuffer = realloc(lineBuffer, (segmentIndex+1)*sizeof(char*));
                lineBuffer[segmentIndex] = segmentBuffer;

                // Set memory for next segment
                segmentMemory = 8;
                segmentBuffer = malloc(segmentMemory);
                segmentIndex++;

                continue;
            } else {
                lineBegin = false;
            }
        }


        // Prepare for next line when a semicolon is found
        if (ch == *";") {
            // Continue if line is empty
            if (!lineHasData) {
                continue;
            }

            // Reallocate to hold another line pointer 
            lines = realloc(lines, (lineCount+1)*sizeof(char*));

            // Add line sentinel
            //lineBuffer[segmentIndex] = lnsentinel;

            printf("%sghh", segmentBuffer);

            lineBuffer = realloc(lineBuffer, (segmentIndex+1)*sizeof(char*));
            lineBuffer[segmentIndex] = segmentBuffer; // Add last segment

            // Append line to lines
            lines[lineCount] = lineBuffer;

            // Reset buffers for next line
            segmentMemory = 8;
            segmentBuffer = malloc(segmentMemory);
            segmentIndex = 0;

            // Setting the buffer to NULL leaves behind
            // the memory address of the previous line
            // so when realloc is called again, new
            // memory will be created for the next line
            // instead of overwriting the previous line
            lineBuffer = NULL;
            lineCount++;

            lineHasData = false;
            lineBegin = true;
        } else {
            // Append char to line buffer

            int len = strlen(segmentBuffer);

            // Allocate more memory for line segment if needed
            if (segmentMemory < len+1) {
                segmentMemory += 8;
                segmentBuffer = realloc(
                    segmentBuffer, segmentMemory
                );
            }

            segmentBuffer[len] = ch;
            segmentBuffer[len + 1] = '\0';
            lineHasData = true;
        }
    }

    // Add script sentinel
    char** scsentinel = malloc(1);
    scsentinel[0] = "SCPT_END";
    lines[lineCount] = scsentinel;

    return lines;
}


// Execute internal Bronze command (make, mod, log, etc...)
void executeInternal(char** obj) {

}


int main() {
    char*** main = parse("sample.bze");

    printf("%p fhh", main);

    int currentLine;
    char** line;

    int currentSegment;
    char* segment;

    currentLine = 0;
    while (true) {
        line = main[currentLine];

        printf("%s %s\n", line[0], line[1]);

        // Break if sentinel (end of script) is found
        if (strcmp(line[0], "SCPT_END") == 0) {
            printf("\nScript finished\n");
            break;
        }

        currentLine++;
    }

    return 0;
}