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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "./coredata.h"
#include "./utils.h"
#include "./setup.h"
#include "backbuff.h"

CoreData DATA;

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
			DATA.backbuff = realloc(DATA.backbuff, backbuffSize(&DATA));
		}

		RenderFrame();

		write(STDOUT_FILENO, DATA.backbuff, backbuffSize(&DATA));

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

	free(DATA.backbuff);

	tcsetattr(STDIN_FILENO, TCSANOW, &DATA.old_termios);

	write(STDOUT_FILENO, "\033[?25h", 6);
	write(STDOUT_FILENO, "\033[?1049l", 8);
}

int main(int argc, char** argv) {
	Setup(&argc, &argv);

	Randix();

	CleanUp();
	return EXIT_SUCCESS;
}
