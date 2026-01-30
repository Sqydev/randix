#ifndef RANDIX_COREDATA_H
#define RANDIX_COREDATA_H

#include <signal.h>
#include <sys/ioctl.h>

typedef struct {
	struct winsize termdim;

	char* backbuff;

	struct {
		volatile sig_atomic_t SIG_INT_TRIGGERED;
		volatile sig_atomic_t SIG_WINCH_TRIGGERED;

		struct sigaction SIG_INT;
		struct sigaction SIG_WINCH;
	} sigs;
	
	struct {
		double refreshRate;
	} args;

	struct {
		double previousTime;
		double currentTime;
		double frameTime;
	} time;
} CoreData;

extern CoreData DATA;

#endif
