#ifndef RANDIX_BACKBUFF_H
#define RANDIX_BACKBUFF_H

#include <stddef.h>

#include "./coredata.h"

static inline size_t backbuffSize(const CoreData *DATA) {
	return (
    	((DATA->termdim.ws_col + 1) * DATA->termdim.ws_row)
    	+ DATA->termdim.ws_col
    	+ 3  /* \033[H */
    	+ 3  /* \033[J */
    ) * sizeof(char);
}

#endif
