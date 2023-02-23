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

static pthread_t drumThreadID;
static void* drumBeatThread(void *vargp);
bool drum_isPlaying;

static wavedata_t* drumKitPlayer;

void Drum_startPlaying()
{
    drumKitPlayer = AudioMixer_getDrumkit();
    AudioMixer_Druminit();
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

void Drum_setMode(int newMode)
{
    current = newMode;
}

void Drum_nextMode()
{
    if (current+1 > 2)
        current = 0;
    else
        current++;
}

// conversion to wait for half a beat, as given in description
static long drumBeat_timeForHalfBeat()
{
	int active_bpm = AudioMixer_getBPM();
    double bps = SEC_PER_MIN/(double)active_bpm;
    bps = bps/2.0;
    return bps * MS_PER_SEC;
}

// DRUM PLAYSOUND

void Drum_playSound(int drumVal)
{
    if (drumVal == 0)
        AudioMixer_queueSound(&drumKitPlayer[0]);
    else if (drumVal == 1)
        AudioMixer_queueSound(&drumKitPlayer[1]);
    else    
        AudioMixer_queueSound(&drumKitPlayer[2]);
}


/// DRUM MODES:

void Drum_off(){
    //play nothing

    //repeat
}

//rock beat as described in section 1 of as3
void Drum_rock()
{
    long play_quarter_note_time = drumBeat_timeForHalfBeat();

    printf("sleep time: %li\n", play_quarter_note_time);
    
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

// https://youtu.be/4lgYw5DOZx8 themed after Rush's Tom Sawyer
void Drum_custom()
{
    long play_quarter_note_time = drumBeat_timeForHalfBeat();

    //1: base
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //1.25 
    //no sound
    sleepForMs(play_quarter_note_time);
    //1.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //1.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //2 hihat + snare
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[2]);
    sleepForMs(play_quarter_note_time);
    //2.25 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //2.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //2.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //3 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //3.25 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //3.5 hihat+base
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //3.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //4 hihat + snare
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[2]);
    sleepForMs(play_quarter_note_time);
    //4.25 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //4.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //4.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //needs at least three different drums

    //5: base+hihat 
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //5.25 hihat 
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //5.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //5.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //6 hihat + snare
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[2]);
    sleepForMs(play_quarter_note_time);
    //6.25 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //6.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //6.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //7 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //7.25 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //7.5 hihat+base
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //7.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //8 hihat + snare
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[2]);
    sleepForMs(play_quarter_note_time);
    //8.25 hihat +base
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);
    //8.5 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    AudioMixer_queueSound(&drumKitPlayer[1]);
    sleepForMs(play_quarter_note_time);
    //8.75 hihat
    AudioMixer_queueSound(&drumKitPlayer[0]);
    sleepForMs(play_quarter_note_time);

}

//plays the drum beat according to the selected mode
static void* drumBeatThread(void *vargp)
{
    printf("Drum Cycle thread starting!\n");
    drum_isPlaying = true;
    while(drum_isPlaying)
    {
        
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
        // sleep(1);
    }
    printf("Mode cycle thread ending\n");
    return 0;
}

