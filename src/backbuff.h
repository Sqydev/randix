#ifndef RANDIX_BACKBUFF_H
#define RANDIX_BACKBUFF_H

#include <stddef.h>

#include "./coredata.h"


static inline size_t backbuffSize(const CoreData *DATA) {
    size_t bytesPerChar = 24; 
	size_t total = bytesPerChar * DATA->termdim.ws_col * DATA->termdim.ws_row;

    total += 6; // \033[H\033[J

    return total;
}


#endif
