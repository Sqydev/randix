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
