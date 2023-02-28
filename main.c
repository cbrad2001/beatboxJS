#include <alsa/asoundlib.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "include/input_joystick.h"
#include "include/udp.h"
#include "include/terminal.h"
#include "include/drumBeats.h"
#include "include/audioMixer.h"
#include "include/periodTimer.h"
#include "include/accelerometer.h"

// main running code that calls each pertaining module's thread. 
// starts and stops relevant threads
int main()
{
	Period_init();
	AudioMixer_init();
	Joystick_startListening();
	Accel_start();
	udp_startSampling();
	Drum_startPlaying();
	Terminal_startPrinting();


	Terminal_stopPrinting();
	Drum_stopPlaying();
	udp_stopSampling();
	Joystick_stopListening();
	AudioMixer_cleanup();
	Period_cleanup();
	// Accel_stop(); // already called in the shutdown function in udp.c

	printf("Done!\n");
	return 0;
}
