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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "./coredata.h"
#include "./backbuff.h"

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


