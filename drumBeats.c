#include "include/drumBeats.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "include/audioMixer_template.h"

typedef enum{
    off,
    rock,
    custom
} drum_mode;

static pthread_t drumThreadID;
static void* drumBeatThread(void *vargp);
bool drum_isPlaying;

void Drum_startPlaying()
{
    drum_isPlaying = true;
    pthread_create(&drumThreadID,NULL,&drumBeatThread,NULL);
}

void Drum_stopPlaying()
{
    drum_isPlaying = false;
    pthread_join(drumThreadID,NULL);
}

void Drum_quit()
{
    drum_isPlaying = false;
}

/// DRUM MODES:

void Drum_off(){
    //play nothing
}

void Drum_rock()
{
    //section 1:
    // loop (corresponding to half beat) that repeats
    // time for half beat = 60 (sec/min) / BPM / 2 (half-beats per beat)


}


void Drum_custom()
{
    //needs at least three different drums


}

//plays the drum beat according to the selected mode
static void* drumBeatThread(void *vargp)
{
    drum_isPlaying = true;
    while(drum_isPlaying)
    {
        //if mode = 0 
        // play nothing
        //if mode = 1
        // rock beat
        //if mode = 2
        // custom beat
    }
    printf("Mode cycle thread ending\n");
    return 0;
}

