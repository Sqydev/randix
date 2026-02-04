#ifndef RANDIX_BACKBUFF_H
#define RANDIX_BACKBUFF_H

#include <stddef.h>

#include "./coredata.h"


static inline size_t backbuffSize(const CoreData *DATA) {
	/*
	 * Longest possible thing:
	 * "\033[48;2;255;255;255m;38;2;255;255;255mX"
	*/
	const size_t bytesPerChar = 50;

	size_t chars =
		(size_t)DATA->termdim.ws_col *
		(size_t)DATA->termdim.ws_row;

	size_t total = bytesPerChar * chars;

	// Newline
	total += (size_t)(DATA->termdim.ws_row - 1);

	// Home "\033[H"
	total += 3;

	// Color reset "\033[0m"
	total += 4;

	// \0
	total += 1;

	return total;
}


#endif
