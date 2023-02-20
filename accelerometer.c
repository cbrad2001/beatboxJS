#include "include/accelerometer.h"
#include "include/helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2CDRV_LINUX_BUS "/dev/i2c-1" // this is the i2c bus for the accelerometer
#define ACCEL_12C_ADDR "0x1C" // this is the address of the accelerometer on the i2c bus
#define ACCEL_CTRL_REG "0x2A" // this is the register to set to enable the accelerometer

// definitions for MSB register addresses for X, Y, and Z axes
#define X_MSB_ADDR ((unsigned char)0x01)
#define Y_MSB_ADDR ((unsigned char)0x03)
#define Z_MSB_ADDR ((unsigned char)0x05)

// definitions for MSB threshold values
// these are values gathered after a short test, so adjust accordingly
#define X_POS_THRESHOLD ((unsigned char)0x40)
#define X_NEG_THRESHOLD ((unsigned char)0xba)
#define Y_POS_THRESHOLD ((unsigned char)0xa0)
#define Y_NEG_THRESHOLD ((unsigned char)0x40)
#define Z_POS_THRESHOLD ((unsigned char)0x75) // under 1G of gravity MSB reads "40"
#define Z_NEG_THRESHOLD ((unsigned char)0x15)

static void* accelThread(void *vargp);

static int initI2cBus(char *bus, int address);
static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);
static void runCommand(char *command);

static pthread_t accelThreadID;
static bool isRunning;


void Accel_start(void)
{
    runCommand("config-pin P9_17 i2c");
    runCommand("config-pin P9_18 i2c");
    sleepForMs(350);

    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS, ACCEL_12C_ADDR);
    isRunning = true;
    pthread_create(&accelThreadID, NULL, accelThread, &i2cFileDesc);
}

void Accel_stop(void)
{
    runCommand("config-pin P9_17 default");
    runCommand("config-pin P9_18 default");
    isRunning = false;
    pthread_join(accelThreadID, NULL);
}


static void* accelThread(void *vargp)
{
    int i2cFileDesc = *(int*)vargp;
    while (isRunning)
    {
        unsigned char xMsbVal = readI2cReg(i2cFileDesc, X_MSB_ADDR);
        unsigned char yMsbVal = readI2cReg(i2cFileDesc, Y_MSB_ADDR);
        unsigned char zMsbVal = readI2cReg(i2cFileDesc, Z_MSB_ADDR);

        // TODO: add debounce

        if (xMsbVal == X_POS_THRESHOLD || xMsbVal == X_NEG_THRESHOLD)
        {
            // queue the audio file to play here
        }

        if (yMsbVal == Y_POS_THRESHOLD || yMsbVal == Y_NEG_THRESHOLD)
        {

        }

        if (zMsbVal == Z_POS_THRESHOLD || zMsbVal == Z_NEG_THRESHOLD)
        {
            
        }
    }
}

// provided by I2C guide
static int initI2cBus(char *bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("Unable to set the I2C device to slave address");
        exit(-1);
    }
    return i2cFileDesc;
}

// provided by I2C guide
static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr))
    {
        perror("I2C: Unable to write to i2c register for reading.");
        exit(1);
    }

    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value))
    {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

// provided by I2C guide
static void runCommand(char *command)
{
    // Execute the shell command (output into pipe)    
	FILE *pipe = popen(command, "r");    
	// Ignore output of the command; but consume it     
	// so we don't get an error when closing the pipe.    
	char buffer[1024];    
	while (!feof(pipe) && !ferror(pipe)) 
	{        
		if (fgets(buffer, sizeof(buffer), pipe) == NULL)            
			break;        
		// printf("--> %s", buffer);  // Uncomment for debugging    
	}    
	// Get the exit code from the pipe; non-zero is an error:    
	int exitCode = WEXITSTATUS(pclose(pipe));    
	if (exitCode != 0) 
	{        
		perror("Unable to execute command:");        
		printf("  command:   %s\n", command);        
		printf("  exit code: %d\n", exitCode);    
	} 
}
