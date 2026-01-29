#ifndef RANDIX_COREDATA_H
#define RANDIX_COREDATA_H

#include <sys/ioctl.h>

typedef struct {
	struct winsize termdim;

	char* backbuff;
	
	struct {
		int refreshRate;
	} args;

	struct {
		double previousTime;
		double currentTime;
		double frameTime;
	} time;
} CoreData;

extern CoreData DATA;

#endif
