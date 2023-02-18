#include "include/input_joystick.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "include/helpers.h"
#include "include/audioMixer.h"

#define JOYSTICK_DEBOUNCE_MS 100

typedef enum {    
    up,         //26
    down,       //46
    left,       //65
    right,      //47
    mid,        //27
    none
} joystick_direction;

static pthread_t joystickThreadID;
static void* joystickThread(void *vargp);
bool joystick_isRunning;


void Joystick_startListening()
{
    joystick_isRunning = true;
    pthread_create(&joystickThreadID,NULL,&joystickThread,NULL);
}

void Joystick_stopListening()
{
    joystick_isRunning = false;
    pthread_join(joystickThreadID,NULL);
}

void Joystick_quit()
{
    joystick_isRunning = false;
}

//
// static members: 
//

//write to file (hardcoded to write "in" to the joystick direciton)
static void setStickToInput(char * joystickFile) 
{
    FILE *pFile = fopen(joystickFile, "w"); 
    if (pFile == NULL) { 
        printf("ERROR: Unable to open file (%s) for read\n", joystickFile); 
        exit(-1); 
    } 
    fprintf(pFile,"in");         //joystick pin set to input
    fclose(pFile);  
}

/////////////////

// exports the joystick and sets direction / value to running
static void init_joystick(void){


    runCommand("config-pin p8.14 gpio"); //up
    runCommand("config-pin p8.15 gpio"); //right
    runCommand("config-pin p8.16 gpio"); //down
    runCommand("config-pin p8.18 gpio"); //left 
    //mid

    setStickToInput(down_dir);
    setStickToInput(left_dir);
    setStickToInput(right_dir);
    setStickToInput(up_dir);
    // setStickToInput(mid_dir);

}

//determines if the given joystick path has been activated
static bool joystick_pressed(char* joystickFile)
{
    FILE *pFile = fopen( joystickFile, "r"); 
    if (pFile == NULL) { 
        printf("ERROR: Unable to open file (%s) for read\n", joystickFile); 
        exit(-1); 
    } 
    // Read string (line) 
    const int MAX_LENGTH = 1024; 
    char press_val[MAX_LENGTH]; 
    fgets(press_val, MAX_LENGTH, pFile);    //puts output into buff as a string
    fclose(pFile);

    if (atol(press_val) == 0LL)
        return true; 
        
    return false;
}

// local check of joystick_pressed()
//returns a direction for each movement of the joystick
//when pressed, holds value 0. Constantly look for value 0 and return that joystick
static joystick_direction determine_press()
{
    //check each file's value at each press
    if (joystick_pressed(gpio_up)) return up;
    else if (joystick_pressed(gpio_down)) return down;
    else if (joystick_pressed(gpio_left)) return left;
    else if (joystick_pressed(gpio_right)) return right;
    else if (joystick_pressed(gpio_mid)) return mid;
    else return none;
}


// Constantly listens for interaction with the joystick, and performs an action based on the action
static void* joystickThread(void *vargp)
{
    int currentVol, newVol, currentTempo, newTempo;
    init_joystick();
    printf("Starting thread for joystick listener\n");
    joystick_isRunning = true;


    while(joystick_isRunning)
    {
        joystick_direction current = determine_press();
        switch(current){

            case up:
                //increase volume by 5
                currentVol = AudioMixer_getVolume();
                newVol = currentVol + 5;
                AudioMixer_setVolume(newVol);
                printf("Changing volume to: %d/%d\n", AudioMixer_getVolume(), AUDIOMIXER_MAX_VOLUME);
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case down:
                //decrease volume by 5
                currentVol = AudioMixer_getVolume();
                newVol = currentVol - 5;
                AudioMixer_setVolume(newVol);
                printf("Changing volume to: %d/%d\n", AudioMixer_getVolume(),AUDIOMIXER_MAX_VOLUME);
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case right:
                //increase tempo by 5bpm
                currentTempo = AudioMixer_getBPM();
                newTempo = currentTempo + 5;
                AudioMixer_setBPM(newTempo);
                printf("Changing tempo to: %d/%d BPM\n", AudioMixer_getBPM(),MAX_BPM);
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case left:
                //decrease tempo by 5bpm
                currentTempo = AudioMixer_getBPM();
                newTempo = currentTempo - 5;
                AudioMixer_setBPM(newTempo);
                printf("Changing tempo to: %d/%d BPM\n", AudioMixer_getBPM(),MAX_BPM);
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case mid:
                //cycle through the beats/modes (not yet implemented)
                break;

            case none:
                //keep cycling
                break;

            default: 
                //default here is none:
                break;

        }
    }
    printf("Ending joystick thread execution\n");
    return 0;



}

