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

#include <time.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "./coredata.h"
#include "signal.h"

CoreData DATA;

double GetTime() {
	struct timespec time_struct;
	clock_gettime(CLOCK_MONOTONIC, &time_struct);

	return time_struct.tv_sec + time_struct.tv_nsec / 1e9;
}

void SIG_INT_HANDLER(int sig) {
	(void)sig;
	DATA.sigs.SIG_INT_TRIGGERED = 1;
}

void SIG_WINCH_HANDLER(int sig) {
	(void)sig;
	DATA.sigs.SIG_WINCH_TRIGGERED = 1;
}

void SignalsSetup() {
	DATA.sigs.SIG_INT_TRIGGERED = 0;
	DATA.sigs.SIG_WINCH_TRIGGERED = 0;

	sigaction(SIGINT, NULL, &DATA.sigs.SIG_INT);

	struct sigaction sia = { 0 };
	sia.sa_handler = SIG_INT_HANDLER;

	sigemptyset(&sia.sa_mask);
	sia.sa_flags = 0;
	sigaction(SIGINT, &sia, NULL);


	sigaction(SIGWINCH, NULL, &DATA.sigs.SIG_WINCH);

	struct sigaction swa = { 0 };
	swa.sa_handler = SIG_WINCH_HANDLER;

	sigemptyset(&swa.sa_mask);
	swa.sa_flags = 0;
	sigaction(SIGWINCH, &swa, NULL);
}

void Setup(int *argc, char** *argv) {
	SignalsSetup();

	DATA.args.refreshRate = 62; // In ms

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
		  		"	-r, --refresh-rate[N]      Set refresh rate of the animation to N milliseconds.\n"
		  		, 7 + 18 + 9 + 38 + 72);
				exit(EXIT_SUCCESS);

			case 'r':
				DATA.args.refreshRate = atoi(optarg);
				if(DATA.args.refreshRate <= 0) { DATA.args.refreshRate = 1; }

				break;
			
			default:
                write(STDOUT_FILENO, "Unknown option, type -h or --help to get help\n", 47);
				exit(EXIT_FAILURE);
		}
	}

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &DATA.termdim);

	DATA.backbuff = malloc((((DATA.termdim.ws_col + 1) * DATA.termdim.ws_row) + DATA.termdim.ws_col + 3 /* this one is for \033[H */ + 3 /* for \033[J */) * sizeof(char));
	
	DATA.time.previousTime = 0;
	DATA.time.currentTime = 0;

	DATA.time.frameTime = DATA.args.refreshRate / 1000.0;

	write(STDOUT_FILENO, "\033[?1049h", 8);
	write(STDOUT_FILENO, "\033[?25l", 6);
	write(STDOUT_FILENO, "\033[2J", 4);
}

void RenderFrame() {
	memcpy(DATA.backbuff, "\033[H\033[J", 6);

	for(int y = 0; y < DATA.termdim.ws_row; y++) {
		for(int x = 0; x < DATA.termdim.ws_col; x++) {
			DATA.backbuff[3 + 3 + y * (DATA.termdim.ws_col + 1) + x] = rand() % 95 + 32;
		}
		if(y != DATA.termdim.ws_row - 1) {
			DATA.backbuff[3 + 3 + y * (DATA.termdim.ws_col + 1) + DATA.termdim.ws_col] = '\n';
		}
	}
}

void Randix() {
	while(1) {
		double frameStart = GetTime();

		if(DATA.sigs.SIG_INT_TRIGGERED == 1) {
			DATA.sigs.SIG_INT_TRIGGERED = 0;
			break;
		}
		if(DATA.sigs.SIG_WINCH_TRIGGERED) {
			DATA.sigs.SIG_WINCH_TRIGGERED = 0;

			ioctl(STDOUT_FILENO, TIOCGWINSZ, &DATA.termdim);
			DATA.backbuff = realloc(DATA.backbuff, (((DATA.termdim.ws_col + 1) * DATA.termdim.ws_row) + DATA.termdim.ws_col + 3 /* this one is for \033[H */ + 3 /* for \033[J */) * sizeof(char));
		}

		RenderFrame();

		write(STDOUT_FILENO, DATA.backbuff, 3 + 3 + DATA.termdim.ws_row * (DATA.termdim.ws_col + 1));

		double frameEnd = GetTime();

		double sleepTime = DATA.time.frameTime - (frameEnd - frameStart);
		if(sleepTime > 0) {
			struct timespec time_struct;
			time_struct.tv_sec = (time_t)sleepTime;
			// sleepTime - sleepTime but tv_sec one will be clamped to nearest int
			time_struct.tv_nsec = (sleepTime - time_struct.tv_sec) * 1e9;

			nanosleep(&time_struct, NULL);
		}
	}
}

void CleanUp() {
	sigaction(SIGINT, &DATA.sigs.SIG_INT, NULL);
	sigaction(SIGINT, &DATA.sigs.SIG_WINCH, NULL);

	write(STDOUT_FILENO, "\033[?25h", 6);
	write(STDOUT_FILENO, "\033[?1049l", 8);
}

int main(int argc, char** argv) {
	Setup(&argc, &argv);

	Randix();

	CleanUp();
	return EXIT_SUCCESS;
}
