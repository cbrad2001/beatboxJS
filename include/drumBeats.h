// Determines the proper drum mode and calls the play to that beat sequence
#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

#define BASE_DRUM   "/mnt/remote/myApps/beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT_DRUM "100053__menegass__gui-drum-cc.wav"
#define SNARE_DRUM  "100059__menegass__gui-drum-snare-soft.wav"

// Start/Stop thread that actively listens to the mode of the drumming 
void Drum_startPlaying();
void Drum_stopPlaying();

void Drum_quit();

//drum modes:
void Drum_off();
void Drum_rock();
void Drum_custom();

#endif
