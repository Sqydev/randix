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

int RenderFrame() {
	if (!DATA.backbuff) return 0;

	int posPointer = 0;

	posPointer += snprintf(DATA.backbuff + posPointer, backbuffSize(&DATA) - posPointer, "\033[H");

	for(int y = 0; y < DATA.termdim.ws_row; y++) {
		for(int x = 0; x < DATA.termdim.ws_col; x++) {
			char har = rand() % 95 + 32;
			
			switch(DATA.args.colorsQuality) {
				case 0: {
					posPointer += snprintf(
						DATA.backbuff + posPointer,
						backbuffSize(&DATA) - posPointer,
						"%c", har
					);
					
					break;
				}
				case 1: {
					/*
					 * Ok, so. in this esc seq:
					 * For fg: 30 is black, 31 is red and so on up to 37
					 * For bg its the same but + 10. So 40 is black, 41 is red and so on up to 47.
					*/

					unsigned char color = (rand() % 8) + 30;
					if(DATA.args.colorsType > 1) {
						if(DATA.args.colorsType == 2) {
							color += 10;
						}
						else {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[%dm", (rand() % 8) + 40
							);
						}
					}

					if(DATA.args.colorsType != 4) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm%c", color, har
						);
					}
					else {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm ", color + 10
						);
					}

					break;
				}
				case 2: {
					/*
					 * Same as in case 1
					*/
					
					unsigned char color = (rand() % 8) + 30;
					unsigned char colorB = (rand() % 8) + 90;
					bool colorDeterment = rand() % 2;

					if(colorDeterment == true) {
						color = colorB;
					}

					if(DATA.args.colorsType > 1) {
						if(DATA.args.colorsType == 2) {
							color += 10;
						}
						else {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[%dm", (rand() % 8) + 40
							);
						}
					}

					if(DATA.args.colorsType != 4) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm%c", color, har
						);
					}
					else {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm ", color + 10
						);
					}
					
					break;
				}
				case 3: {
					unsigned char color = rand() % 256;

					switch(DATA.args.colorsType) {
						case 1: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[38;5;%dm%c", color, har
							);

							break;
						}
						case 2: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;5;%dm%c", color, har
							);

							break;
						}
						case 3: {
							unsigned char bgColor = rand() % 256;

							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;5;%d;38;5;%dm%c", color, bgColor, har
							);

							break;
						}
						case 4: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;5;%dm ", color
							);

							break;
						}
					}

					break;
				}
				case 4: {
					unsigned char r = rand() % 256;
					unsigned char g = rand() % 256;
					unsigned char b = rand() % 256;


					switch(DATA.args.colorsType) {
						case 1: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[38;2;%d;%d;%dm%c", r, g, b, har
							);

							break;
						}
						case 2: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;2;%d;%d;%dm%c", r, g, b, har
							);

							break;
						}
						case 3: {
							unsigned char bgR = rand() % 256;
							unsigned char bgG = rand() % 256;
							unsigned char bgB = rand() % 256;

							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;2;%d;%d;%d;38;2;%d;%d;%dm%c", r, g, b, bgR, bgG, bgB, har
							);

							break;
						}
						case 4: {
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[48;2;%d;%d;%dm ", r, g, b
							);

							break;
						}
					}

					break;
				}
			}
		}

		if(y != DATA.termdim.ws_row - 1) {
			posPointer += snprintf(DATA.backbuff + posPointer, backbuffSize(&DATA) - posPointer, "\n");
		}
	}
	posPointer += snprintf(DATA.backbuff + posPointer, backbuffSize(&DATA) - posPointer, "\033[0m");

	return posPointer;
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

		// PinPointer becouse it sounds funny
		int pinPointer = RenderFrame();
		write(STDOUT_FILENO, DATA.backbuff, pinPointer);

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

	write(STDOUT_FILENO, "\033[0m", 3);
	write(STDOUT_FILENO, "\033[?25h", 6);
	write(STDOUT_FILENO, "\033[?1049l", 8);
}

int main(int argc, char** argv) {
	Setup(&argc, &argv);

	Randix();

	CleanUp();
	return EXIT_SUCCESS;
}
