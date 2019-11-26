#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>

#include "timer.h"
#include "Serial.h"
#include "signal.h"


struct Signal signal;

void reset()
{
    signal.index = 0;
    int i;
    for(i=0; i < 33; i++)
        signal.arr[i] = 0;
}

int complete()
{
    return signal.index == 32;
}

void add(int b)
{
    if(complete(signal))
        return ;
    signal.arr[signal.index] = b;
    signal.index++;
}

//requires !complete()
//converts a Signal to integer
unsigned int convert()
{
    int result = 0, i =0;
    for(; i < 32; i++)
    {
        result = (result << 1) +( signal.arr[i] & 1);
    }
    return result;
}
