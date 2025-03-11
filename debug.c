int debug_CurrentLine = 0;
char* debug_CurrentFile;

void except(char* message) {
    printf(
        "\n\n\033[1;31mAn exception occurred in file %s on line %i:\n%s\n\n\033[0m", 
        debug_CurrentFile, debug_CurrentLine, message
    );

    exit(1);
}