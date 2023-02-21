#include "include/udp.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <limits.h>
#include <netdb.h>
#include <unistd.h>	
#include <float.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/socket.h>

#include "include/drumBeats.h"
#include "include/input_joystick.h"
#include "include/audioMixer.h"
#include "include/terminal.h"
#include "include/drumBeats.h"

/**
 * 
 *  must support:
 *      changing drum beat mode
 *      changing volume
 *      changing tempo
 *      playing a specific sound from within the beat
 * 
*/

#define PORT 12345      // RUN: netcat -u 192.168.7.2 12345                          
#define MAX_LEN 1024

#define CMD_HELP    "help\n"
#define CMD_MODE    "mode\n"
#define CMD_VOLUME  "volume\n"
#define CMD_TEMPO   "tempo\n"     
#define CMD_SOUND   "sound"        
#define CMD_STOP    "stop\n"
#define ENTER       '\n'

static pthread_t udpThreadID;

static struct sockaddr_in sock;
static int socketDescriptor;
static bool isConnected;
static socklen_t sock_sz;

static void* udpCommandThread(void *vargp);

void udp_startSampling(void)
{
    isConnected = true;
    pthread_create(&udpThreadID, NULL, &udpCommandThread, NULL);
}

void udp_stopSampling(void)
{
    isConnected = false;
    pthread_join(udpThreadID, NULL);
}

static void* udpCommandThread(void *vargp)
{
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = htonl(INADDR_ANY);
    sock.sin_port = htons(PORT);

    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1)
    {
        perror("Can't create socket\n");
        exit(1);
    }

    if (bind (socketDescriptor, (struct sockaddr*) &sock, sizeof(sock)) == -1)
    {
        perror("Failed to bind socket\n");
        exit(1);
    }
    sock_sz = sizeof(sock);
    char recvBuffer[MAX_LEN];
    char cmdHistory[MAX_LEN];
    char sendBuffer[MAX_LEN];

    printf("UDP Thread Starting\n");
    isConnected = true;
    while(isConnected){ 
        int bytesRx = recvfrom(socketDescriptor, recvBuffer, MAX_LEN, 0, (struct sockaddr*)&sock, &sock_sz);
        if (bytesRx == -1 )                             // create recipient buffer, check if connected
        {
            printf("Can't receive data \n");
            exit(1);
        } 

        if (recvBuffer[0] == ENTER)                       
        {
            strncpy(recvBuffer, cmdHistory, MAX_LEN);   // call prev command if 'enter'
        }
        strncpy(cmdHistory,recvBuffer,MAX_LEN);         //store history for enter command

        if (strncmp(recvBuffer, CMD_HELP,4)==0)         // String data appended as a helpful output message
        {
            sprintf(sendBuffer, "Accepted command examples:\n");
			strcat(sendBuffer, "mode #      -- Set drum mode. {0 = off, 1 = rock, 2 = custom}.\n");
			strcat(sendBuffer, "volume ##   -- Update the current volume (## = {00-99}).\n");
			strcat(sendBuffer, "tempo ##    -- Update the current tempo BPM (## = {00-99}).\n");
			strcat(sendBuffer, "sound #     -- Play any one of the sounds used in the drum beat. (0-2)\n");
			strcat(sendBuffer, "stop        -- Cause the server program to end.\n");
      strcat(sendBuffer, "<enter>     -- Repeat last command.\n");

			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }

        else if (strncmp(recvBuffer, CMD_MODE, 4) == 0) //Set to 1 of 3 modes
        {
            char *startOfN = recvBuffer + 4;            // 4th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < 0 || n > 3 ){                       // invalid mode
                char *errMsg = "Choose one of the valid 3 modes. {0 = off, 1 = rock, 2 = custom}";
                sprintf(sendBuffer, "%s.\n", errMsg);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                Drum_setMode(n);
                sprintf(sendBuffer, "Successfully set mode to %d.\n", Drum_getMode());
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
        }

        else if (strncmp(recvBuffer, CMD_VOLUME, 6) ==0)//Update volume
        {
            char *startOfN = recvBuffer + 6;            // 6th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < 0 || n > 100 ){                      // invalid volume
                char *errMsg = "Invalid volume. Set to between 0-99";
                sprintf(sendBuffer, "%s.\n", errMsg);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                AudioMixer_setVolume(n);
                sprintf(sendBuffer, "Successfully set volume to %d.\n", AudioMixer_getVolume());
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }  
        }
        else if (strncmp(recvBuffer, CMD_TEMPO, 5) ==0) // Update tempo (bpm)
        {
            char *startOfN = recvBuffer + 5;            // 5th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < MIN_BPM || n > MAX_BPM ){         // invalid tempo
                char *errMsg = "Invalid tempo. Set to between ";
                sprintf(sendBuffer, "%s%d-%d.\n", errMsg,MIN_BPM,MAX_BPM);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                AudioMixer_setBPM(n);
                sprintf(sendBuffer, "Successfully set tempo to %dbpm.\n", AudioMixer_getBPM());
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            
        }
        else if (strncmp(recvBuffer, CMD_SOUND, 5) ==0) // Play a sound
        {
            char *startOfN = recvBuffer + 5;            // 5th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < 0 || n > 2 ){                       // invalid sound
                char *errMsg = "Invalid sound. Set to ";
                sprintf(sendBuffer, "%s%d-%d.\n", errMsg,0,2);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                wavedata_t tempSound = AudioMixer_getDrumkit()[n];
                AudioMixer_queueSound(&tempSound);
                sprintf(sendBuffer, "Successfully played sound #%d.\n", n);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
        }

        else if (strncmp(recvBuffer, CMD_STOP,4) == 0)  // shuts off all threads; quits local udp and remote sampler
        {
            sprintf(sendBuffer, "Program terminating: (enter to quit)\n");
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            Joystick_quit();
            Drum_quit();
            Terminal_quit();
            // STOP ALL RELEVANT THREADS HERE
            isConnected = false;
        }
        else                                            // default case: unknown
        {
            sprintf(sendBuffer, "Unknown command.\n");
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        memset(recvBuffer, 0, MAX_LEN);
        memset(sendBuffer, 0, MAX_LEN);










    }
    close(socketDescriptor);
    printf("UDP thread ending!\n");
    return 0;
}