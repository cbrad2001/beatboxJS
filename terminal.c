#include "include/audioMixer.h"
#include "include/periodTimer.h"
#include "include/drumBeats.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

static pthread_t termThreadID;
static bool isPrinting;
static long long prevSampleSum; 

static void* print_to_terminal(void *vargp);

void Terminal_startPrinting(void)
{
    isPrinting = true;
    prevSampleSum = 0;
    pthread_create(&termThreadID, NULL, &print_to_terminal, NULL);
}

void Terminal_stopPrinting(void)
{
    isPrinting = false;
    pthread_join(termThreadID, NULL);
}

void Terminal_quit()
{
    isPrinting = false;
}

//thread repeatedly outputs specified data to screen at a 1 second rate
static void* print_to_terminal(void *vargp)
{
    printf("Terminal thread starting!\n");
    isPrinting = true;
    while(isPrinting){  //collect relevant information from other modules for reporting.
        Period_statistics_t audioStats; // accelStats;      
        Period_getStatisticsAndClear(PERIOD_EVENT_PLAYBACK_BUFFER, &audioStats);    //fill stats struct with data
        // Period_getStatisticsAndClear(PERIOD_EVENT_ACCELEROMETER, &accelStats); 
        // Mode ##bpm vol:## Audio[{min}, {max}] avg {avg}/{num-samples}   

        int bpm = AudioMixer_getBPM();
        int vol = AudioMixer_getVolume();
        int mode = Drum_getMode();

        //Accel[{min}, {max}] avg {avg}/{num-samples} 

        printf("M%d\t" 
               "%dbpm\t" 
               "vol:%d\t" 
               "Audio[%.3f, %.3f] avg %.3f/%i\n", 
            //    "Accel[%.3f, %.3f] avg %.3f/%lli\n",
                    mode,
                    bpm,
                    vol,
                    audioStats.minPeriodInMs, audioStats.maxPeriodInMs, audioStats.avgPeriodInMs,audioStats.numSamples);
                    // accelStats.minPeriodInMs, accelStats.maxPeriodInMs, accelStats.avgPeriodInMs,accelStats.numSamples);
                    

        sleep(1);       //write every second
    }
    printf("Terminal thread ending!\n");
    return 0;
}