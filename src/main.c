/*
 * Randix - random characters terminal filler
 * Copyright (C) 2026-present Sqydev/_Sqyd_/Sqyd/Wojciech
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Original repo: https://github.com/Sqydev/randix.git
 * Link to author: https://github.com/Sqydev
*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	struct winsize termdim; 
	
	struct {
		int refreshRate;
	} args;
} CoreData;

CoreData DATA;

void Setup(int *argc, char** *argv) {
	DATA.args.refreshRate = 500; // In ms

	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"refresh-rate", required_argument, NULL, 'r'},
		{NULL, 0, NULL, 0}
	};

	int optchar;
	while((optchar = getopt_long(*argc, *argv, "hr:", long_options, NULL)) != EOF) {
		switch(optchar) {
			case 'h':
				write(STDOUT_FILENO, 
		  		"Usage:\n"
		  		"randix [options]\n\n"
		  		"Options:\n"
		  		"	-h, --help                 Get help.\n"
		  		"	-r, --refresh-rate[N]      Set refresh rate of the animation to N fps.\n"
		  		, 7 + 18 + 9 + 38 + 72);
				exit(EXIT_SUCCESS);

			case 'r':
				DATA.args.refreshRate = atoi(optarg);
				break;
			
			default:
                write(STDOUT_FILENO, "Unknown option, type -h or --help to get help\n", 47);
				exit(EXIT_FAILURE);
		}
	}

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &DATA.termdim);

	write(STDOUT_FILENO, "\033[?1049h", 8);
	write(STDOUT_FILENO, "\033[?25l", 6);
}

void Randix() {
	printf("terminal: %dx%d\n", DATA.termdim.ws_row, DATA.termdim.ws_col);
	printf("rr: %d\n", DATA.args.refreshRate);
	fflush(stdout);
	sleep(1);
}

void Cleanup() {
	write(STDOUT_FILENO, "\033[?25h", 6);
	write(STDOUT_FILENO, "\033[?1049l", 8);
}

int main(int argc, char** argv) {
	Setup(&argc, &argv);

	Randix();

	Cleanup();
	return EXIT_SUCCESS;
}
