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

#include <stdlib.h>

#include "./coredata.h"
#include "./signals.h"

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
