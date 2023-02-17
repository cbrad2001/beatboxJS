// Determines the proper drum mode and calls the play to that beat sequence
#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

// Start/Stop thread that actively listens to the mode of the drumming 
void Drum_startPlaying();
void Drum_stopPlaying();

void Drum_quit();

//drum modes:
void Drum_off();
void Drum_rock();
void Drum_custom();

#endif
