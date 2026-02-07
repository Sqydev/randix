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
#include "signal.h"

CoreData DATA;

int RenderFrame() {
	if (!DATA.backbuff) return 0;

	int posPointer = 0;

	posPointer += snprintf(DATA.backbuff + posPointer, backbuffSize(&DATA) - posPointer, "\033[H");

	for(int y = 0; y < DATA.termdim.ws_row; y++) {
		for(int x = 0; x < DATA.termdim.ws_col; x++) {
			unsigned char har;

			if(!DATA.args.charList || DATA.args.charList[0] == '\0') {
				har = rand() % 95 + 32;
			}
			else {
				har = DATA.args.charList[rand() % strlen(DATA.args.charList)];
			}
			
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
					unsigned char color;

					if(!DATA.args.colorList) {
						color = (rand() % 8) + 30;
					}
					else {
						color = DATA.args.colorList[rand() % DATA.args.colorListLen].color + 30;
					}

					if(DATA.args.colorsType > 1) {
						if(DATA.args.colorsType == 2) {
							color += 10;
						}
						else {
							unsigned char colorBg;
							if(!DATA.args.colorList) {
								colorBg = (rand() % 8) + 40;
							}
							else {
								colorBg = DATA.args.colorList[rand() % DATA.args.colorListLen].color + 40;
							}
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[%d", colorBg
							);
						}
					}

					if(DATA.args.colorsType < 3) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm%c", color, har
						);
					}
					else if(DATA.args.colorsType == 4) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm ", color + 10
						);
					}
					else {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							";%dm%c", color, har
						);
					}

					break;
				}
				case 2: {
					/*
					 * Same as in case 1
					*/
					unsigned char color;

					if(!DATA.args.colorList) {
						color = (rand() % 8) + 30;
						unsigned char colorB = (rand() % 8) + 90;
						
						bool colorDeterment = rand() % 2;

						if(colorDeterment == true) {
							color = colorB;
						}
					}
					else {
						int place = rand() % DATA.args.colorListLen;
						if(DATA.args.colorList[place].color > 7) {
							color = DATA.args.colorList[place].color + 90 - 8;
						}
						else {
							color = DATA.args.colorList[place].color + 30;
						}
					}
					

					if(DATA.args.colorsType > 1) {
						if(DATA.args.colorsType == 2) {
							color += 10;
						}
						else {
							unsigned char colorBg;
							if(!DATA.args.colorList) {
								colorBg = (rand() % 8) + 40;
							}
							else {
								colorBg = DATA.args.colorList[rand() % DATA.args.colorListLen].color + 40;
							}
							posPointer += snprintf(
								DATA.backbuff + posPointer,
								backbuffSize(&DATA) - posPointer,
								"\033[%d", colorBg
							);
						}
					}

					if(DATA.args.colorsType < 3) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm%c", color, har
						);
					}
					else if(DATA.args.colorsType == 4) {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							"\033[%dm ", color + 10
						);
					}
					else {
						posPointer += snprintf(
							DATA.backbuff + posPointer,
							backbuffSize(&DATA) - posPointer,
							";%dm%c", color, har
						);
					}
					
					break;
				}
				case 3: {
					unsigned char color;
					if(!DATA.args.colorList) {
						color = rand() % 256;
					}
					else {
						color = DATA.args.colorList[rand() % DATA.args.colorListLen].color;
					}

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
							unsigned char bgColor;
							if(!DATA.args.colorList) {
								bgColor = rand() % 256;
							}
							else {
								bgColor = DATA.args.colorList[rand() % DATA.args.colorListLen].color;
							}

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
					unsigned char r;
					unsigned char g;
					unsigned char b;
					if(!DATA.args.colorList) {
						r = rand() % 256;
						g = rand() % 256;
						b = rand() % 256;
					}
					else {
						int color = rand() % DATA.args.colorListLen;
						r = DATA.args.colorList[color].r;
						g = DATA.args.colorList[color].g;
						b = DATA.args.colorList[color].b;
					}

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
							unsigned char bgR;
							unsigned char bgG;
							unsigned char bgB;
							if(!DATA.args.colorList) {
								bgR = rand() % 256;
								bgG = rand() % 256;
								bgB = rand() % 256;
							}
							else {
								int color = rand() % DATA.args.colorListLen;
								bgR = DATA.args.colorList[color].r;
								bgG = DATA.args.colorList[color].g;
								bgB = DATA.args.colorList[color].b;
							}

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
	for(;;) {
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
	sigaction(SIGWINCH, &DATA.sigs.SIG_WINCH, NULL);

	free(DATA.backbuff);
	DATA.backbuff = NULL;
	free(DATA.args.charList);
	DATA.args.charList = NULL;
	free(DATA.args.colorList);
	DATA.args.colorList = NULL;

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
