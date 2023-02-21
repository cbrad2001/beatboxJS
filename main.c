#include <alsa/asoundlib.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "include/input_joystick.h"
#include "include/udp.h"
#include "include/terminal.h"
#include "include/drumBeats.h"
#include "include/audioMixer.h"
#include "include/periodTimer.h"

// #define SAMPLE_RATE   44100
// #define NUM_CHANNELS  1
// #define SAMPLE_SIZE   (sizeof(short)) 	// bytes per sample
// #define SOURCE_FILE "/mnt/remote/myApps/beatbox-wav-files/100060__menegass__gui-drum-splash-hard.wav"
// #define SOURCE_FILE_2 "/mnt/remote/myApps/beatbox-wav-files/100056__menegass__gui-drum-cyn-hard.wav"

// Store data of a single wave file read into memory.
// Space is dynamically allocated; must be freed correctly!
// typedef struct {
// 	int numSamples;
// 	short *pData;
// } wavedata_t;

// // Prototypes:
// snd_pcm_t *Audio_openDevice();
// void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct);
// void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData);

int main()
{
    //initialize audio mixer 
    //while(true) {   
        // play the base drum sound   
        // sleep(1); 
    //}
	Period_init();
	AudioMixer_init();
	Joystick_startListening();
	udp_startSampling();
	Drum_startPlaying();
	Terminal_startPrinting();
	

    // printf("Beginning play-back of %s\n", SOURCE_FILE);

	// // Configure Output Device
	// snd_pcm_t *handle = Audio_openDevice();

	// // Load wave file we want to play:
	// wavedata_t sampleFile;
	// Audio_readWaveFileIntoMemory(SOURCE_FILE, &sampleFile);

	// wavedata_t sampleFile2;
	// Audio_readWaveFileIntoMemory(SOURCE_FILE_2, &sampleFile2);

	// // Play Audio./i	
	// for (int i = 0; i < 5; i++) {
	// 	Audio_playFile(handle, &sampleFile);
	// 	Audio_playFile(handle, &sampleFile2);

	// 	// Drop and prepare to prevent underrun (need to stop buffer from reading with drop)
	// 	snd_pcm_drop(handle);
	// 	sleep(1);
	// 	snd_pcm_prepare(handle);
	// }

	// // Cleanup, letting the music in buffer play out (drain), then close and free.
	// snd_pcm_drain(handle);
	// snd_pcm_hw_free(handle);
	// snd_pcm_close(handle);
	// free(sampleFile.pData);
	// free(sampleFile2.pData);

	Terminal_stopPrinting();
	Drum_stopPlaying();
	udp_stopSampling();
	Joystick_stopListening();
	AudioMixer_cleanup();
	Period_cleanup();

	printf("Done!\n");
	return 0;
}

// Open the PCM audio output device and configure it.
// Returns a handle to the PCM device; needed for other actions.
// snd_pcm_t *Audio_openDevice()
// {
// 	snd_pcm_t *handle;

// 	// Open the PCM output
// 	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
// 	if (err < 0) {
// 		printf("Play-back open error: %s\n", snd_strerror(err));
// 		exit(EXIT_FAILURE);
// 	}

// 	// Configure parameters of PCM output
// 	err = snd_pcm_set_params(handle,
// 			SND_PCM_FORMAT_S16_LE,
// 			SND_PCM_ACCESS_RW_INTERLEAVED,
// 			NUM_CHANNELS,
// 			SAMPLE_RATE,
// 			1,			// Allow software resampling
// 			50000);		// 0.05 seconds per buffer
// 	if (err < 0) {
// 		printf("Play-back configuration error: %s\n", snd_strerror(err));
// 		exit(EXIT_FAILURE);
// 	}

// 	return handle;
// }

// // Read in the file to dynamically allocated memory.
// // !! Client code must free memory in wavedata_t !!
// void Audio_readWaveFileIntoMemory(char *fileName, wavedata_t *pWaveStruct)
// {
// 	assert(pWaveStruct);

// 	// Wave file has 44 bytes of header data. This code assumes file
// 	// is correct format.
// 	const int DATA_OFFSET_INTO_WAVE = 44;

// 	// Open file
// 	FILE *file = fopen(fileName, "r");
// 	if (file == NULL) {
// 		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
// 		exit(EXIT_FAILURE);
// 	}

// 	// Get file size
// 	fseek(file, 0, SEEK_END);
// 	int sizeInBytes = ftell(file) - DATA_OFFSET_INTO_WAVE;
// 	fseek(file, DATA_OFFSET_INTO_WAVE, SEEK_SET);
// 	pWaveStruct->numSamples = sizeInBytes / SAMPLE_SIZE;

// 	// Allocate Space
// 	pWaveStruct->pData = malloc(sizeInBytes);
// 	if (pWaveStruct->pData == NULL) {
// 		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
// 				sizeInBytes, fileName);
// 		exit(EXIT_FAILURE);
// 	}

// 	// Read data:
// 	int samplesRead = fread(pWaveStruct->pData, SAMPLE_SIZE, pWaveStruct->numSamples, file);
// 	if (samplesRead != pWaveStruct->numSamples) {
// 		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
// 				pWaveStruct->numSamples, fileName, samplesRead);
// 		exit(EXIT_FAILURE);
// 	}

// 	fclose(file);
// }

// // Play the audio file (blocking)
// void Audio_playFile(snd_pcm_t *handle, wavedata_t *pWaveData)
// {
// 	// If anything is waiting to be written to screen, can be delayed unless flushed.
// 	fflush(stdout);

// 	// Write data and play sound (blocking)
// 	snd_pcm_sframes_t frames = snd_pcm_writei(handle, pWaveData->pData, pWaveData->numSamples);

// 	// Check for errors
// 	if (frames < 0)
// 		frames = snd_pcm_recover(handle, frames, 0);
// 	if (frames < 0) {
// 		fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
// 		exit(EXIT_FAILURE);
// 	}
// 	if (frames > 0 && frames < pWaveData->numSamples)
// 		printf("Short write (expected %d, wrote %li)\n", pWaveData->numSamples, frames);
// }