#include "include/input_joystick.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "include/audioMixer_template.h"

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




// given functions from as1:

static void runCommand(char* command) 
{    
    // Execute the shell command (output into pipe)    
    FILE *pipe = popen(command, "r");    
    // Ignore output of the command; but consume it     
    // so we don't get an error when closing the pipe.    
    char buffer[1024];    
    while (!feof(pipe) && !ferror(pipe)) {        
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)            
            break;        
        // printf("--> %s", buffer);  
        // Uncomment for debugging    
    }    
    // Get the exit code from the pipe; non-zero is an error:    
    int exitCode = WEXITSTATUS(pclose(pipe));    
    if (exitCode != 0) {        
        perror("Unable to execute command:");        
        printf("  command:   %s\n", command);        
        printf("  exit code: %d\n", exitCode);    
    } 
}

static void sleepForMs(long long delayInMs) 
{    
    const long long NS_PER_MS = 1000 * 1000;    
    const long long NS_PER_SECOND = 1000000000;    
    long long delayNs = delayInMs * NS_PER_MS;    
    int seconds = delayNs / NS_PER_SECOND;    
    int nanoseconds = delayNs % NS_PER_SECOND;    
    struct timespec reqDelay = {seconds, nanoseconds};    
    nanosleep(&reqDelay, (struct timespec *) NULL); 
}

// static long long getTimeInMs(void) 
// {    
//     struct timespec spec;    
//     clock_gettime(CLOCK_REALTIME, &spec);    
//     long long seconds = spec.tv_sec;    
//     long long nanoSeconds = spec.tv_nsec;    
//     long long milliSeconds = seconds * 1000                  
//                 + nanoSeconds / 1000000;    
//     return milliSeconds; 
// }

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


////


static void* joystickThread(void *vargp)
{
    int currentVol, newVol; //, currentBMP, newBMP;
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
                printf("Volume: %d\n", AudioMixer_getVolume());
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case down:
                //decrease volume by 5
                currentVol = AudioMixer_getVolume();
                newVol = currentVol - 5;
                AudioMixer_setVolume(newVol);
                printf("Volume: %d\n", AudioMixer_getVolume());
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case right:
                //increase tempo by 5bpm
                sleepForMs(JOYSTICK_DEBOUNCE_MS);
                break;

            case left:
                //decrease tempo by 5bpm
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
