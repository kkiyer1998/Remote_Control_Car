#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"

/*
    * Provides a vector data structure with maximum size
    * 32.
    * The client can use this to manipulate signals as they are
    * read.
*/

struct Signal
{
    int arr[32];
    int index;
};

//resets the signal
//the signal is empty after the function returns
void reset(struct Signal *);

//checks if the signal is complete
//checks whether we have read 32 bits
int complete(struct Signal *);

//adds a bit to the end of the signal
void add(struct Signal *,int);

//requires !complete()
//converts a Signal to integer
unsigned int convert(struct Signal *);

#endif
