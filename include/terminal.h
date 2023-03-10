// terminal.h
// Module to write output to the screen.
// Converts information into terminal readings
// 
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

// Begin/end the background thread which prints to screen using printf()
void Terminal_startPrinting(void);
void Terminal_stopPrinting(void);

//trigger udp shutdown - swap loop state to false for exit
void Terminal_quit(void);

#endif