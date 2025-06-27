#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG 
#include <stdint.h>

void error(char *filename, uint32_t line);

#define ASSERT(toCheck) \
if(!(toCheck)) {                                     \
    error(__FILE__, __LINE__);                   \
}
                                                    
#else
#define ASSERT(toCheck)
#endif

#endif
