// Determines the proper drum mode and calls the play to that beat sequence
#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

// Start/Stop thread that actively listens to the mode of the drumming 
void Drum_startPlaying();
void Drum_stopPlaying();

void Drum_quit();

//retrieves current mode
int Drum_getMode();
void Drum_setMode(int newMode);
void Drum_nextMode();

//drum modes:
void Drum_off();
void Drum_rock();
void Drum_custom();

#endif
