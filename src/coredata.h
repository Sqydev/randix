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

#ifndef RANDIX_COREDATA_H
#define RANDIX_COREDATA_H

#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdbool.h>

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char color;
} Color;

typedef struct {
	struct winsize termdim;
	struct termios old_termios;
	struct termios new_termios;

	char* backbuff;
	char* version;

	int writeCode;

	struct {
		volatile sig_atomic_t SIG_INT_TRIGGERED;
		volatile sig_atomic_t SIG_WINCH_TRIGGERED;

		struct sigaction SIG_INT;
		struct sigaction SIG_WINCH;
	} sigs;
	
	struct {
		double refreshRate;
		int colorsQuality;
		int colorsType;

		char* charList;
		Color* colorList;
		int colorListLen;
	} args;

	struct {
		double previousTime;
		double currentTime;
		double frameTime;
	} time;
} CoreData;

extern CoreData DATA;

#endif
