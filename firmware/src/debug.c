#include "debugUART.h"

#ifdef DEBUG
#include "debug.h"
void error(char *pcFilename, uint32_t line) {
    debugUART_printString("ERROR in file: ");
    debugUART_printString(pcFilename);
    debugUART_printString(":");
    debugUART_printWord(line);
    debugUART_printChar('\n');
}
#endif
