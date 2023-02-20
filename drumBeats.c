#include "include/drumBeats.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "include/audioMixer.h"
#include "include/helpers.h"
#include "include/periodTimer.h"

typedef enum{
    off,
    rock,
    custom
} drum_mode;

drum_mode current;
static int active_bpm;

static pthread_t drumThreadID;
static void* drumBeatThread(void *vargp);
bool drum_isPlaying;

wavedata_t* drumKitPlayer;

void Drum_startPlaying()
{
    drumKitPlayer = AudioMixer_getDrumkit();
    AudioMixer_Druminit();
    active_bpm = AudioMixer_getBPM();
    drum_isPlaying = true;
    current = 1;            //temp default
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

int Drum_getMode()
{
    return (int)current;
}

// conversion to wait for half a beat, as given in description
static long drumBeat_timeForHalfBeat(int bpm)
{
	long time = ((double)SEC_PER_MIN / bpm / 2); 
	return time;
}

/// DRUM MODES:

void Drum_off(){
    //play nothing
}

//rock beat as described in section 1 of as3
void Drum_rock()
{
    active_bpm = AudioMixer_getBPM();
    long play_quarter_note_time = drumBeat_timeForHalfBeat(active_bpm) * MS_PER_SEC;
    
    //1: hihat + base
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //1.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //2 hihat+snare
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[2]);
    sleepForMs(play_quarter_note_time);
    // sleepForMs(play_quarter_note_time)
    //2.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);

    //repeat
}


void Drum_custom()
{
    //needs at least three different drums


}

//plays the drum beat according to the selected mode
static void* drumBeatThread(void *vargp)
{
    printf("Drum Cycle thread starting!\n");
    drum_isPlaying = true;
    while(drum_isPlaying)
    {
        Period_markEvent(PERIOD_EVENT_PLAYBACK_BUFFER);     //THIS MAY NEED TO RELOCATE
        if (current == off)
        {
            Drum_off();
        }
        else if (current == rock)
        {
            Drum_rock();
        }
        else if (current == custom)
        {
            Drum_custom();
        }
        else
        {
            perror("drum mode not selected\n");
            sleep(10);
        }
        sleep(1);
    }
    printf("Mode cycle thread ending\n");
    return 0;
}

