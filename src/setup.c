#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termio.h>

#include "./coredata.h"
#include "./setup.h"
#include "./signals.h"
#include "backbuff.h"
#include "bits/getopt_core.h"

void DataSetup() {
	DATA.args.refreshRate = 62; // In ms
	
	DATA.args.colorsQuality = 1;
	DATA.args.colorsType = 1;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &DATA.termdim);

	DATA.backbuff = malloc(backbuffSize(&DATA));
	
	DATA.time.previousTime = 0;
	DATA.time.currentTime = 0;

	DATA.time.frameTime = DATA.args.refreshRate / 1000.0;
}

void OptsSetup(int *argc, char** *argv) {
	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"refresh-rate", required_argument, NULL, 'r'},
		{"color-quality", required_argument, NULL, 't'},
		{"color-type", required_argument, NULL, 't'},
		{NULL, 0, NULL, 0}
	};

	int optchar;
	while((optchar = getopt_long(*argc, *argv, "hr:cq:t:", long_options, NULL)) != EOF) {
		switch(optchar) {
			case 'h': {
				const char* help =
					"Usage:\n"
					"  randix [options]\n\n"

					"Options:\n"
					"  -h, --help\n"
					"      Show this help and exit.\n\n"
				
					"  -r, --refresh-rate <ms>\n"
					"      Frame refresh rate in milliseconds (min: 1).\n\n"
				
					"  -cq <level>\n"
					"      Color quality / palette:\n"
					"        0  no colors\n"
					"        1  8-color ANSI (standard)\n"
					"        2  16-color ANSI (bright)\n"
					"        3  256-color ANSI\n"
					"        4  TrueColor (24-bit RGB)\n\n"

					"  -ct <mode>\n"
					"      Color application mode:\n"
					"        1  foreground only\n"
					"        2  background only\n"
					"        3  foreground + background\n"
					"        4  background fill (space character)\n\n"
	
					"Examples:\n"
					"  randix -c -q 3 -t 1      256-color foreground\n"
					"  randix -c -q 4 -t 4      TrueColor background fill\n";
				write(STDOUT_FILENO, help, strlen(help));
				exit(EXIT_SUCCESS);
			}

			case 'r': {
				if(optarg == NULL) {
					write(STDOUT_FILENO, "Option -r requires an argument\n", 31);
					exit(EXIT_FAILURE);
				}
				DATA.args.refreshRate = atoi(optarg);
				if(DATA.args.refreshRate <= 0) { DATA.args.refreshRate = 1; }

				DATA.time.frameTime = DATA.args.refreshRate / 1000.0;

				break;
			}

			case 'c': {
				break;
			}

			case 'q': {
				DATA.args.colorsQuality = atoi(optarg);
				if(DATA.args.colorsQuality < 0) { DATA.args.colorsQuality = 0; }
				if(DATA.args.colorsQuality > 4) { DATA.args.colorsQuality = 4; }

				break;
			}

			case 't': {
				DATA.args.colorsType = atoi(optarg);
				if(DATA.args.colorsType < 1) { DATA.args.colorsType = 1; }
				if(DATA.args.colorsType > 4) { DATA.args.colorsType = 4; }

				break;
			}
			
			default: {
				write(STDOUT_FILENO, "Unknown option, type -h or --help to get help\n", 47);
				exit(EXIT_FAILURE);
			}
		}
	}
}

void Setup(int *argc, char** *argv) {
	SignalsSetup(); // In signals.c becouse it's a bit more complicated
	
	DataSetup();

	OptsSetup(argc, argv);

	write(STDOUT_FILENO, "\033[?1049h", 8);
	write(STDOUT_FILENO, "\033[?25l", 6);
	write(STDOUT_FILENO, "\033[2J", 4);

	tcgetattr(STDIN_FILENO, &DATA.old_termios);

	DATA.new_termios = DATA.old_termios;
	DATA.new_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &DATA.new_termios);
}
