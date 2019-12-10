#ifndef ET_H
#define ET_H

#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"

struct expectedTime
{
    int startHigh;
    int startLow;
    int low;
    int hi0;
    int hi1;
};

int bitRead(int,int);

#endif
