#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "encoder.c"

size_t strlen(const char *str)
{
    const char *s;
    for (s = str; *s; ++s);
    return(s - str);
}

// Read and refine script files
char*** parse(char* filename) {
    // Read script file
    FILE* fptr;
    fptr = fopen(filename, "r");

    FILE* out;
    out = fopen("out.c", "a");


    char* lineSentinel = malloc(5);
    lineSentinel = "LEND";


    // Will store pointers for each line
    char*** lines = NULL; // Where's my award?
    int lineCount = 0;

    // This will be toggled if the reader comes
    // across a comment symbol (`)
    bool comment_ignore = false;

    // This will be toggled if the reader comes
    // across parentheses, telling it to ignore spaces in-between them.
    bool ignore_delimiters = false;
    
    bool line_started = true;
    int chunk_encoding = 0; // Defaults to utf-8 (0)
    
    int segment_index = 0; // Index of current segment of current line being written to
    int segment_mem = 8; // Default memory given to a segment
    int segment_len = 0;
    char* segment_buffer = malloc(segment_mem);

    char** lineBuffer = NULL; //malloc(lineMemory);
    char ch;
    while (ch != EOF) {
        ch = fgetc(fptr);

        // Ignore newlines
        if (ch == *"\n") {
            continue;
        }
        
        // Toggle comment_ignore if there is a comment symbol
        if (ch == *"`") {
            comment_ignore = !comment_ignore;
            continue;
        }
        // Ignore following chars if comment_ignore is true
        if (comment_ignore) {
            continue;
        }

        // Enable/disable ignore_delimiters if there are parentheses
        if (ch == *"(") {
            ignore_delimiters = true;
            continue;
        } if (ch == *")") {
            ignore_delimiters = false;
            continue;
        }

        if (ch == *" ") {
            if (line_started) {
                continue;
            }
        } else {
            line_started = false;
        }


        if (!ignore_delimiters) {
            // Colons separate a type declaration from
            // the actual value. The syntax of a given
            // value to a command should be written as
            // the type followed by a colon and then the
            // value (e.g. int:4). If there is no type
            // declaration, the value is interpreted
            // as a string by default.

            if (ch == *":") {
                // Since the "chunk" variable is built up
                // character by character, if a colon is found
                // the chunk variable must contain the type name.

                chunk_encoding = 1;

                //printf("%s\n", segment_buffer);

                //chunk_type = match chunk.as_str() {
                //    "str" => 0, // String (0) (declaration optional)
                //    "int" => 1, // Integer (1)
                //    "float" => 2, // Float (2)
                //    "bool" => 3, // Boolean (3)
                //    _ => 4, // Default to string (0)
                //};

                // Since the chunk is built char by char, Clearing the chunk
                // at this point will make it so that when the delimiter code
                // below is triggered, the chunk will not contain the declaration
                // but the raw value only.
                segment_mem = 8;
                segment_len = 0;
                segment_buffer = malloc(segment_mem);

                continue;
            }


            // Delimiters are spaces and semicolons
            // A semicolon represents the end of a line
            // A space represents the end of a chunk unless between parentheses
            // This checks if the char is one of these delimiters: 
            if ((ch == *" ") || (ch == *";")) {
                // If a delimiter has been found, it means
                // the current chunk of the line should be processed
                // and added to the line buffer.

                // Allocate space in line for another segment pointer
                lineBuffer = realloc(lineBuffer, (segment_index+2)*sizeof(char*));
                lineBuffer[segment_index] = segment_buffer; // Add segment

                printf("%s %i ", segment_buffer, segment_index);

                // THIS IS WHERE CHUNKS SHOULD BE ENCODED BASED ON ENCODING.

                // Let's compile to C
                // When incl is seen, it should read a bzec (command file)
                // And add it as a function in the compiled C code





                // Set memory for next segment
                segment_mem = 8;
                segment_len = 0;
                segment_buffer = malloc(segment_mem);
                segment_index++;

                chunk_encoding = 0;


                // Append line if the delimiter is a semicolon
                if (ch == *";") {
                    // Reallocate to hold another line pointer 
                    lines = realloc(lines, (lineCount+1)*sizeof(char*));

                    // Add line sentinel
                    lineBuffer[segment_index] = lineSentinel;

                    // Append line to lines
                    lines[lineCount] = lineBuffer;

                    //if strcmp(lineBuffer[0], )

                    fprintf(out, "%s(", lineBuffer[0]);
                    printf("\n");

                    segment_index = 0;

                    // Setting the buffer to NULL leaves behind
                    // the memory address of the previous line
                    // so when realloc is called again, new
                    // memory will be created for the next line
                    // instead of overwriting the previous line
                    lineBuffer = NULL;
                    lineCount++;

                    line_started = true;
                }

                continue;
            }
        }


        // Append char to segment buffer

        //int len = stringLength(segment_buffer);

        // Allocate more memory for line segment if needed
        if (segment_mem < segment_len+1) {
            segment_mem += 8;
            segment_buffer = realloc(
                segment_buffer, segment_mem
            );
        }

        segment_buffer[segment_len] = ch;
        segment_buffer[segment_len + 1] = '\0';
        segment_len++;
    }


    // Add script sentinel
    char** scsentinel = malloc(1);
    scsentinel[0] = "SCPT_END";
    lines[lineCount] = scsentinel;

    fclose(fptr);
    fclose(out);

    return lines;
}