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
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	struct winsize termdim;

	char* backbuff;
	
	struct {
		int refreshRate;
	} args;
} CoreData;

CoreData DATA;

void Setup(int *argc, char** *argv) {
	DATA.args.refreshRate = 125; // In ms

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

	DATA.backbuff = malloc(
		(((DATA.termdim.ws_col + 1) * DATA.termdim.ws_row)
		+ DATA.termdim.ws_col
		+ 3 /* this one is for \033[H */
		+ 3 /* for \033[J */)
		* sizeof(char));

	write(STDOUT_FILENO, "\033[?1049h", 8);
	write(STDOUT_FILENO, "\033[?25l", 6);
	write(STDOUT_FILENO, "\033[2J", 4);
}

void Randix() {
	while(1) {
		memcpy(DATA.backbuff, "\033[H\033[J", 6);

		for(int y = 0; y < DATA.termdim.ws_row; y++) {
			for(int x = 0; x < DATA.termdim.ws_col; x++) {
				DATA.backbuff[3 + 3 + y * (DATA.termdim.ws_col + 1) + x] = rand() % 95 + 32;
			}
			if(y != DATA.termdim.ws_row - 1) {
				DATA.backbuff[3 + 3 + y * (DATA.termdim.ws_col + 1) + DATA.termdim.ws_col] = '\n';
			}
		}

		write(STDOUT_FILENO, DATA.backbuff, 3 + 3 + DATA.termdim.ws_row * (DATA.termdim.ws_col + 1));

		usleep(1000 * DATA.args.refreshRate);
	}
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
