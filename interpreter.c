#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "debug.c"
#include "parser.c"

// NOTYPE: A programming language that doesn't have types, but only deals with binary
// Upon parsing, all values will be converted to binary and the types will not be stored
// Functions will take any binary, but expect a certain encoding

// Values will start with something like utf8 or int16 to tell the parser what encoding to
// use


/*

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

// Maybe make a function that parses values in batches
// It might take all arguments and parse them each
// using an iterator and store them in a struct.
// maybe built in commands can accept this struct
// as an argument. Ok brb I gotta go make dinner


void* parseValue(char* raw) {
    char typekey[4];
    strncpy(typekey, raw, 3);
    typekey[3] = '\0';

    int rawlen = strlen(raw);
    rawlen -= 4;

    char* rawValue = malloc(rawlen);
    strncpy(rawValue, raw+4, rawlen);

    printf("%s", rawValue);

    if (strcmp(typekey, "str") == 0) {
        return rawValue;
    }
}

// Execute internal Bronze command (make, mod, log, etc...)
void executeInternal(char** command) {
    char* name = command[0];

    //char* section;
    //int iter = 1;
    //while (true) {
    //    section = command[iter];

    //    iter++;
    //}

    //if (strcmp(name, "log") == 0) {
    //    printf("%s\n", parseValue(command[1]).strval);
    //}
}


int main() {
    //parseValue("str:example");

    //registerCmd("make", 0, 0);

    // == storage ==
    int** commands;
    // =============

    // Parse Issue:
    // Line sentinel is not added to lines that appear after empty newlines
    // This causes strange data put at the beginning of the following line
    // This only happens with longer lines
    // Probably because space is not being allocated for the line sentinel


    char*** main = parse("main.bze");
    debug_CurrentFile = "main.bze";

    int currentLine;
    char** line;

    int currentSegment;
    char* segment;

    // == Line processing variables ==
    char* cmdname;
    char* rawValue;
    int rawLength;
    int sectionIter;
    char** args;
    // ===============================

    // Iterate line by line
    currentLine = 0;
    while (true) {
        line = main[currentLine];
        cmdname = line[0];
        args = line+1;

        //printf("%s %s %s\n", line[0], line[1], line[2]);

        // Break if sentinel (end of script) is found
        if (strcmp(line[0], "SCPT_END") == 0) {
            printf("\nScript finished\n");
            break;
        }

        //if (strcmp(cmdname, "log") == 0) {
        //    internal_log(args);
        //}

        // Iterate through sections of line
        //sectionIter = 0;
        //while (true) {
        //    rawValue = line[sectionIter];
        //    if (strcmp(rawValue, "LEND") == 0) {
        //        break;
        //    }

        //    printf("%s ", rawValue);

        //    // If the value has a type declaration before it,
        //    // It should have a semicolon after the 3 byte type name
        //    if (rawValue[3] == *":") {
        //        printf("type\n");

        //        args[sectionIter] = malloc((rawLength+1)*sizeof(char));
        //        strcpy(args[sectionIter], rawValue);
        //    } else {
        //        // If there is no proper type declaration,
        //        // interpret value as a string.

        //        printf("no type declaration\n");

        //        args[sectionIter] = malloc((rawLength+1)*sizeof(char));
        //        strcpy(args[sectionIter], rawValue);

        //    }

        //    printf("%s", args[0]);

        //    sectionIter++;
        //}

        currentLine++;
        debug_CurrentLine = currentLine+1;
    }

    return 0;
}