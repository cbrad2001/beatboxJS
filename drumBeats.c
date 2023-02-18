#include "include/drumBeats.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "include/audioMixer_template.h"
#include "include/helpers.h"

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

// static wavedata_t hihat, baseDrum, snareDrum;
// static snd_pcm_t *handle;

// static void Drum_init()
// {
// 	// Configure Output Device
// 	handle = Audio_openDevice();

// 	// Load wave file we want to play:
// 	Audio_readWaveFileIntoMemory(HI_HAT_DRUM, &hihat);
// 	Audio_readWaveFileIntoMemory(BASE_DRUM, &baseDrum);
// 	Audio_readWaveFileIntoMemory(SNARE_DRUM, &snareDrum);
// }

// // Cleanup, letting the music in buffer play out (drain), then close and free.
// static void Drum_cleanup()
// {
// 	snd_pcm_drain(handle);
// 	snd_pcm_hw_free(handle);
// 	snd_pcm_close(handle);
// 	free(hihat.pData);
// 	free(baseDrum.pData);
//     free(snareDrum.pData);
// }

void Drum_startPlaying()
{
    // Drum_init();
    active_bpm = AudioMixer_getBPM();
    drum_isPlaying = true;
    pthread_create(&drumThreadID,NULL,&drumBeatThread,NULL);
}

void Drum_stopPlaying()
{
    // Drum_cleanup();
    drum_isPlaying = false;
    pthread_join(drumThreadID,NULL);
}

void Drum_quit()
{
    drum_isPlaying = false;
}

// conversion to wait for half a beat, as given in description
// static int drumBeat_timeForHalfBeat(int bpm)
// {
// 	double time = SEC_PER_MIN / bpm / 2; 
// 	int int_time = (int)time; 
// 	return int_time;
// }

/// DRUM MODES:

void Drum_off(){
    //play nothing
}

//rock beat as described in section 1 of as3
void Drum_rock()
{
    // active_bpm = AudioMixer_getBPM();
    // int play_quarter_note_time = drumBeat_timeForHalfBeat(active_bpm) * MS_PER_SEC;
    //1: hihat + base


    //1.5 hihat
        //play sound
    // sleepForMs(play_quarter_note_time)
    //2 hihat+snare
        //play sound
    // sleepForMs(play_quarter_note_time)
    //2.5 hihat
        //play sound
    // sleepForMs(play_quarter_note_time)


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
    }
    printf("Mode cycle thread ending\n");
    return 0;
}

