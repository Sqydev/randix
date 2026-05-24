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

#include <time.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <unistd.h>

#include "./coredata.h"
#include "./utils.h"
#include "./setup.h"
#include "./backbuff.h"
#include "./signal.h"
#include "./renderFrame.h"

CoreData DATA;

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
		DATA.writeCode = write(STDOUT_FILENO, DATA.backbuff, pinPointer);

		double frameEnd = GetTime();

		double sleepTime = DATA.time.frameTime - (frameEnd - frameStart);
		if(sleepTime > 0) {
			struct timespec time_struct;
			time_struct.tv_sec = (time_t)sleepTime;
			// sleepTime - sleepTime but tv_sec one will be clamped to nearest int
			time_struct.tv_nsec = (sleepTime - time_struct.tv_sec) * 1e9;

			nanosleep(&time_struct, NULL);
		}

		(void)DATA.writeCode;
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

	DATA.writeCode = write(STDOUT_FILENO, "\033[0m", 3);
	DATA.writeCode = write(STDOUT_FILENO, "\033[?25h", 6);
	DATA.writeCode = write(STDOUT_FILENO, "\033[?1049l", 8);
}

int main(int argc, char** argv) {
	Setup(&argc, &argv);

	Randix();

	CleanUp();
	return EXIT_SUCCESS;
}
