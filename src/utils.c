#include <time.h>

#include "utils.h"

double GetTime() {
	struct timespec time_struct;
	clock_gettime(CLOCK_MONOTONIC, &time_struct);

	return time_struct.tv_sec + time_struct.tv_nsec / 1e9;
}
