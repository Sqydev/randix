#ifndef RANDIX_SIGNALS_H
#define RANDIX_SIGNALS_H

void SIG_INT_HANDLER(int sig);
void SIG_WINCH_HANDLER(int sig);
void SignalsSetup();

#endif
