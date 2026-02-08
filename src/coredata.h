#ifndef RANDIX_COREDATA_H
#define RANDIX_COREDATA_H

#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdbool.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char color;
} Color;

typedef struct {
	struct winsize termdim;
	struct termios old_termios;
	struct termios new_termios;

	char* backbuff;
	char* version;

	int writeCode;

	struct {
		volatile sig_atomic_t SIG_INT_TRIGGERED;
		volatile sig_atomic_t SIG_WINCH_TRIGGERED;

		struct sigaction SIG_INT;
		struct sigaction SIG_WINCH;
	} sigs;
	
	struct {
		double refreshRate;
		int colorsQuality;
		int colorsType;

		char* charList;
		Color* colorList;
		int colorListLen;
	} args;

	struct {
		double previousTime;
		double currentTime;
		double frameTime;
	} time;
} CoreData;

extern CoreData DATA;

#endif
