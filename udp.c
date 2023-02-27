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
#include <arpa/inet.h>

#include "include/drumBeats.h"
#include "include/input_joystick.h"
#include "include/audioMixer.h"
#include "include/terminal.h"
#include "include/drumBeats.h"
#include "include/accelerometer.h"

/**
 * 
 *  must support:
 *      changing drum beat mode
 *      changing volume
 *      changing tempo
 *      playing a specific sound from within the beat
 * 
*/

#define BIND_PORT 12345      // RUN: netcat -u 192.168.7.2 12345    
#define NODE_JS_PORT 8089                 
#define MAX_LEN 1024

#define CMD_HELP    "help\n"
#define CMD_MODE    "mode"
#define CMD_VOLUME  "volume"
#define CMD_TEMPO   "tempo"     
#define CMD_SOUND   "sound"        
#define CMD_STOP    "stop"
#define CMD_STATUS  "status"
#define ENTER       '\n'

static pthread_t udpThreadID;
static pthread_t timeThreadID;

static struct sockaddr_in sock;
static int socketDescriptor;
static bool isConnected;
static socklen_t sock_sz;

static void* udpCommandThread(void *vargp);
static void* timeSendThread(void *vargp);

void udp_startSampling(void)
{
    isConnected = true;
    pthread_create(&udpThreadID, NULL, &udpCommandThread, NULL);
    pthread_create(&timeThreadID, NULL, &timeSendThread, NULL);
}

void udp_stopSampling(void)
{
    isConnected = false;
    pthread_join(udpThreadID, NULL);
    pthread_join(timeThreadID, NULL);
}

static void* udpCommandThread(void *vargp)
{
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = htonl(INADDR_ANY);
    sock.sin_port = htons(BIND_PORT);

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

            if (n < 0 || n > 3){                       // invalid mode
                char modeString[10];
                memset(modeString, '\0', 10);
                char *status = "f";
                switch (Drum_getMode()) 
                {
                    case off:
                        strncat(modeString, "off", 9);
                        break;
                    case rock:
                        strncat(modeString, "rock", 9);
                        break;
                    case custom:
                        strncat(modeString, "custom", 9);
                        break;
                }
                // char *errMsg = "mode||Choose one of the valid 3 modes. {0 = off, 1 = rock, 2 = custom}";
                // sprintf(sendBuffer, "%s.\n", errMsg);
                sprintf(sendBuffer, "mode|%s|%s|Choose one of the valid 3 modes. {0 = off, 1 = rock, 2 = custom}", status, modeString);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                char *status = "t";
                Drum_setMode(n);
                char modeString[10];
                memset(modeString, '\0', 10);
                switch(n)
                {
                    case 0:
                        strncat(modeString, "off", 9);
                        break;
                    case 1:
                        strncat(modeString, "rock", 9);
                        break;
                    case 2:
                        strncat(modeString, "custom", 9);
                        break;
                }

                sprintf(sendBuffer, "mode|%s|%s|Successfully set mode to %s.\n", status, modeString, modeString);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
        }

        else if (strncmp(recvBuffer, CMD_VOLUME, 6) ==0)//Update volume
        {
            char *startOfN = recvBuffer + 6;            // 6th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < 0 || n > 100 ){                      // invalid volume
                char *status = "f";
                // char *errMsg = "volume|Invalid volume. Set to between 0-99";
                // sprintf(sendBuffer, "%s.\n", errMsg);
                sprintf(sendBuffer, "volume|%s|Invalid volume. Set to between 0-99", status);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                char *status = "t";
                AudioMixer_setVolume(n);
                int vol = AudioMixer_getVolume();
                sprintf(sendBuffer, "volume|%s|%d|Successfully set volume to %d.\n", status, vol, vol);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }  
        }
        else if (strncmp(recvBuffer, CMD_TEMPO, 5) ==0) // Update tempo (bpm)
        {
            char *startOfN = recvBuffer + 5;            // 5th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < MIN_BPM || n > MAX_BPM ){         // invalid tempo
                char *errMsg = "tempo|f|Invalid tempo. Set to between ";
                sprintf(sendBuffer, "%s%d-%d.\n", errMsg,MIN_BPM,MAX_BPM);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                AudioMixer_setBPM(n);
                int tempo = AudioMixer_getBPM();
                sprintf(sendBuffer, "tempo|t|%d|Successfully set tempo to %d bpm.\n", tempo, tempo);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            
        }
        else if (strncmp(recvBuffer, CMD_SOUND, 5) ==0) // Play a sound
        {
            char *startOfN = recvBuffer + 5;            // 5th position is the start of n
            int n = atoi(startOfN);
            memset(sendBuffer, 0, MAX_LEN);             // 1024 bytes per buffer

            if (n < 0 || n > 2 ){                       // invalid sound
                char *errMsg = "sound|Invalid sound. Set to ";
                sprintf(sendBuffer, "%s%d-%d.\n", errMsg,0,2);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
            else
            {
                // wavedata_t tempSound = AudioMixer_getDrumkit()[n];
                wavedata_t *drumKit = AudioMixer_getDrumkit();
                AudioMixer_queueSound(&drumKit[n]);
                sprintf(sendBuffer, "sound|Successfully played sound #%d.\n", n);
                sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            }
        }

        else if (strncmp(recvBuffer, CMD_STOP,4) == 0)  // shuts off all threads; quits local udp and remote sampler
        {
            sprintf(sendBuffer, "stop|Beatbox shutting down...\n");
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
            Joystick_quit();
            Drum_quit();
            Terminal_quit();
            Accel_stop();
            // STOP ALL RELEVANT THREADS HERE
            isConnected = false;
        }
        else if (strncmp(recvBuffer, CMD_STATUS, 6) == 0) {
            int mode = (int)Drum_getMode();
            int vol = AudioMixer_getVolume();
            int bpm = AudioMixer_getBPM();

            char modeString[10];
            memset(modeString, '\0', 10);
            switch (mode) 
            {
                case 0:
                    strncat(modeString, "off", 9);
                    break;
                case 1:
                    strncat(modeString, "rock", 9);
                    break;
                case 2:
                    strncat(modeString, "custom", 9);
                    break;
            }

            sprintf(sendBuffer, "status|%s|%d|%d\n", modeString, vol, bpm);
            // printf("Debug: sending status as %s\n", sendBuffer);
            sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        else                                            // default case: unknown
        {
            sprintf(sendBuffer, "unknown|Unknown command.\n");
			sendto(socketDescriptor,sendBuffer, strnlen(sendBuffer,MAX_LEN),0,(struct sockaddr *) &sock, sock_sz);
        }
        memset(recvBuffer, 0, MAX_LEN);
        memset(sendBuffer, 0, MAX_LEN);
    }
    close(socketDescriptor);
    printf("UDP thread ending!\n");
    return 0;
}

// Background thread to send uptime.
static void* timeSendThread(void *vargp)
{
    printf("Starting uptime send thread...\n");

    unsigned short destPort = htons(NODE_JS_PORT);
    struct sockaddr_in sendSock;
    int sendSockFd;

    if ((sendSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket error for thread to send time.\n");
        exit(1);
    }

    sendSock.sin_family = AF_INET;
    sendSock.sin_port = destPort;
    sendSock.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (isConnected)
    {
        FILE *uptimeFile = fopen("/proc/uptime", "r");
        if (uptimeFile == NULL)
        {
            perror("Cannot open /proc/uptime\n");
            exit(1);
        }

        char uptimeBuf[MAX_LEN];
        fgets(uptimeBuf, MAX_LEN, uptimeFile);
        fclose(uptimeFile);

        char *uptimeString = strtok(uptimeBuf, ".");

        char sendBuf[MAX_LEN] = "uptime|";
        strncat(sendBuf, uptimeString, MAX_LEN);

        // printf("DEBUG: time thread sending %s\n", sendBuf);
        if (sendto(sendSockFd, sendBuf, strnlen(sendBuf, MAX_LEN), 0,
                (struct sockaddr *)&sendSock, sizeof(sendSock)) < 0)
        {   
            perror("Error sending time string UDP packet\n.");
        }

        sleep(1);
    }

    printf("Uptime send thread stopped.\n");
    close(sendSockFd);
    return 0;
}