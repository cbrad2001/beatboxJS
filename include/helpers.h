// Global helper functions
// Time support
#ifndef HELPERS_H
#define HELPERS_H

#define SEC_PER_MIN 60
#define MS_PER_SEC 1000

// sleep program execution for a given number of ms
void sleepForMs(long long delayInMs);
// long long getTimeInMs(void);

// run a console command
void runCommand(char* command); 

#endif