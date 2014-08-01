

#include <iostream>
using namespace std;

#include <pthread.h>

#include <sched.h>
#include "mgmt.h"

#include <sys/time.h>

#include "ip-utils.h"

void yieldCPU () {
	//sleep(0);
	sched_yield();	
}

void microsleep (unsigned int nMicroSleep, char * pBreakSleep) {
	struct timeval currentTime;
	struct timeval startTime;
	
	// TODO - Add in OS-specific code with #ifdefs to not poll on
	//          longer waits nearly as aggressively
	
	// 1. Get the current system time
	// 2. Adjust the time by the time we want to sleep
	// 3. Loop (yielding each time) until the current time exceeds the exit time
	
	gettimeofday(&startTime, NULL);					
	adjustTime(&startTime, nMicroSleep);
			
	while(1) {
		// At least yield once
		yieldCPU();		
		gettimeofday(&currentTime, NULL);
		
		if(compareTime(&startTime, &currentTime)) {
			// Do nothing, we didn't make it there yet
		} else {
			break;
		}
		
		// If there is a valid memory location for the signal and the signal
		//  memory location is set to a 1, bail out
		//
		// Note that thread safety is not a huge issue in this spot as we are
		// strictly reading the shared location
		if(pBreakSleep != NULL && *pBreakSleep) {
			return;
		}
	}	
}

