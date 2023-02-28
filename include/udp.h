//Module for UDP
// Supports running commands over netcat     
// also enables communication to server by nodejs
#ifndef _UDP_H_
#define _UDP_H_

// Begin/end the background thread which prints to screen using printf()
void udp_startSampling(void);
void udp_stopSampling(void);

#endif
