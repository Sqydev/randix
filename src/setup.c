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

void DataSetup() {
	DATA.args.refreshRate = 62; // In ms
	
	DATA.args.colors = 0;

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
		{"colors", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	int optchar;
	while((optchar = getopt_long(*argc, *argv, "hr:c:", long_options, NULL)) != EOF) {
		switch(optchar) {
			case 'h': {
				const char* help =
					"Usage:\n"
		  			"randix [options]\n\n"
		  			"Options:\n"
		  			"	-h, --help                 Get help.\n"
		  			"	-r, --refresh-rate[N]      Set refresh rate of the animation to N milliseconds.\n"
		  			"	-c, --colors[N]            Make your life colorful and turn on the colors. 0 in non, 1 is 8 colors, 2 is 16, 3 is 255 and 4 in true color mode\n";

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
				if(optarg == NULL) {
					write(STDOUT_FILENO, "Option -c requires an argument\n", 31);
					exit(EXIT_FAILURE);
				}
				DATA.args.colors = atoi(optarg);
				if(DATA.args.colors < 0) { DATA.args.colors = 0; }
				if(DATA.args.colors > 4) { DATA.args.colors = 4; }

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
