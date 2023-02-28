// Determines the proper drum mode and calls the play to that beat sequence
#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

typedef enum drum_mode {    //cycle the modes numerically with more useful enum definitions
    off,
    rock,
    custom
} drum_mode;

// Start/Stop thread that actively listens to the mode of the drumming 
void Drum_startPlaying();
void Drum_stopPlaying();
void Drum_quit();

//retrieves current mode
drum_mode Drum_getMode();
void Drum_setMode(int newMode);
void Drum_nextMode();

//plays 1 individual sound
void Drum_playSound(int drumVal);

//drum modes; performs the functionality of each mode
// each mode performs the beat illustrated in the name:
void Drum_off();
void Drum_rock();
void Drum_custom();

#endif
