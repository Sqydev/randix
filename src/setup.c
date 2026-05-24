/* 
* Copyright (c) 2025-present Wojciech Kaptur ( _Sqyd_ / Sqydev )
* Github: https://github.com/Sqydev
* GPG Fingerprint: 6DC2516B0DFDA9C59661650722F7B8A777F33B56
*
* This software is provided "as-is", without any express or implied warranty. In no event
* will the authors be held liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose, including commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
* 1. Non-Misrepresentation: The origin of this software must not be misrepresented; 
*    you must not claim that you wrote the original software. An acknowledgment in 
*    product documentation is appreciated but not required.
*
* 2. Source-Level Copyleft: Any altered versions (forks) of this software's source code, 
*    or files containing significant portions of this code, must be distributed under 
*    these same license terms. Such modified source code must be made publicly available 
*    to any recipient, even if used over a network (SaaS).
* 
* 3. Proprietary Integration: This software may be integrated into, linked with, or 
*    used as a component of proprietary and closed-source products. In such cases, 
*    the surrounding proprietary application code does not need to be disclosed, 
*    provided that the original or modified source code of THIS software remains 
*    available under the terms of Section 2.
*
* 4. Persistent Metadata: All original credits, including those in the source code headers 
*    and binary metadata (e.g., ELF .comment section, PE StringFileInfo, or equivalent), 
*    must not be removed. You may add your own credits to forks, provided the original 
*    authorship remains clearly identified.
* 
* 5. Notice Retention: This license notice may not be removed or altered from any 
*    source or binary distribution.
*/

#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "./coredata.h"
#include "./setup.h"
#include "./signals.h"
#include "./backbuff.h"
#include "../version.h"
#include "utils.h"

void DataSetup() {
	DATA.args.refreshRate = 62; // In ms
	
	DATA.args.colorsQuality = 1;
	DATA.args.colorsType = 1;

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &DATA.termdim);

	DATA.backbuff = malloc(backbuffSize(&DATA));
	
	DATA.time.previousTime = 0;
	DATA.time.currentTime = 0;

	DATA.time.frameTime = DATA.args.refreshRate / 1000.0;

	DATA.args.charList = NULL;
	DATA.args.colorList = NULL;

	DATA.version = get_version();

	srand(GetTime());
}

void OptsSetup(int *argc, char ***argv) {
	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"refresh-rate", required_argument, NULL, 'r'},
		{"color-quality", required_argument, NULL, 'q'},
		{"color-type", required_argument, NULL, 't'},
		{"palette", required_argument, NULL, 'p'},
		{"char-palette", required_argument, NULL, 'c'},
		{"seed", required_argument, NULL, 's'},
		{NULL, 0, NULL, 0}
	};

	int optchar;
	while((optchar = getopt_long(*argc, *argv, "hvr:q:t:c:p:s:", long_options, NULL)) != -1) {
		switch(optchar) {
			case 'v': {
				DATA.writeCode = write(STDOUT_FILENO, DATA.version, strlen(DATA.version));
				exit(EXIT_SUCCESS);
			}

			case 'h': {
				const char *help =
					"Usage:\n"
					"  randix [options]\n\n"

					"Options:\n"
					"  -h, --help\n"
					"      Show this help and exit.\n\n"

					"  -r, --refresh-rate <int>\n"
					"      Frame refresh rate in milliseconds (min: 1).\n\n"

					"  -q, --color-quality <int>\n"
					"      Color quality / palette:\n"
					"        0  no colors\n"
					"        1  8-color ANSI (standard)\n"
					"        2  16-color ANSI (bright)\n"
					"        3  256-color ANSI\n"
					"        4  TrueColor (24-bit RGB)\n\n"

					"  -t, --color-type <int>\n"
					"      Color application mode:\n"
					"        1  foreground only\n"
					"        2  background only\n"
					"        3  foreground + background\n"
					"        4  background fill (space character)\n\n"

					"  -p, --palette <string>\n"
					"      Specify color palette for randix to choose colors from.\n"
					"      Format should be compatible with used color quality(-q),\n"
					"      Each color should be separated with ',' and each channel should be separated with ;\n"
					"      So for -q 1 you'll do -p 0,1 for colors black and red\n"
					"      But for -q 4 you'll do -p 000;000;000,255;255;255 for colors black and white\n\n"

					"  -c, --char-palette <string>\n"
					"      Make string for randix to choose characters from.\n\n"

					"  -s, --seed <int>\n"
					"      Specyfy seed.\n";

				DATA.writeCode = write(STDOUT_FILENO, help, strlen(help));
				exit(EXIT_SUCCESS);
			}

			case 'r': {
				DATA.args.refreshRate = atoi(optarg);
				if (DATA.args.refreshRate <= 0)
					DATA.args.refreshRate = 1;
				DATA.time.frameTime = DATA.args.refreshRate / 1000.0;
				break;
			}

			case 'q': {
				DATA.args.colorsQuality = atoi(optarg);
				if (DATA.args.colorsQuality < 0) DATA.args.colorsQuality = 0;
				if (DATA.args.colorsQuality > 4) DATA.args.colorsQuality = 4;
				break;
			}

			case 't': {
				DATA.args.colorsType = atoi(optarg);
				if (DATA.args.colorsType < 1) DATA.args.colorsType = 1;
				if (DATA.args.colorsType > 4) DATA.args.colorsType = 4;
				break;
			}

			case 'c': {
				DATA.args.charList = strdup(optarg);
				if (!DATA.args.charList) {
					DATA.writeCode = write(STDERR_FILENO,
						"Memory allocation error for charList\n", 38);
					exit(EXIT_FAILURE);
				}
				break;
			}

			case 'p': {
				int colorCount = 0;
				for(char* pin = optarg; *pin != '\0'; pin++) {
					if(*pin ==',') colorCount++;
				}
				colorCount++;

				DATA.args.colorListLen = colorCount;

				DATA.args.colorList = malloc(colorCount * sizeof(Color));
				if (!DATA.args.colorList) {
					DATA.writeCode = write(STDERR_FILENO,
						"Memory allocation error for colorList\n", 39);
					exit(EXIT_FAILURE);
				}

				char* saveptr1;
				char* colorToken = strtok_r(optarg, ",", &saveptr1);

				int i = 0;
				while(colorToken) {
					char* saveptr2;

					DATA.args.colorList[i].color = (unsigned char)atoi(colorToken);

					char* channel = strtok_r(colorToken, ";", &saveptr2);

					DATA.args.colorList[i].r = channel ? atoi(channel) : 0;
					channel = strtok_r(NULL, ";", &saveptr2);
					DATA.args.colorList[i].g = channel ? atoi(channel) : 0;
					channel = strtok_r(NULL, ";", &saveptr2);
					DATA.args.colorList[i].b = channel ? atoi(channel) : 0;

					i++;
					colorToken = strtok_r(NULL, ",", &saveptr1);
				}

				break;
			}

			case 's': {
				srand(atoi(optarg));

				break;
			}

			default: {
				DATA.writeCode = write(STDERR_FILENO,
					"Unknown option, type -h or --help\n", 33);
				exit(EXIT_FAILURE);
			}
		}
	}
}

void Setup(int *argc, char** *argv) {
	SignalsSetup(); // In signals.c becouse it's a bit more complicated
	
	DataSetup();

	OptsSetup(argc, argv);

	DATA.writeCode = write(STDOUT_FILENO, "\033[?1049h", 8);
	DATA.writeCode = write(STDOUT_FILENO, "\033[?25l", 6);
	DATA.writeCode = write(STDOUT_FILENO, "\033[2J", 4);

	tcgetattr(STDIN_FILENO, &DATA.old_termios);

	DATA.new_termios = DATA.old_termios;
	DATA.new_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &DATA.new_termios);
}
