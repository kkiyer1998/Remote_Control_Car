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
    const int startHigh:4500*80;
    const int startLow:9000*80;
    const int low:560*80;
    const int hi0:560*80;
    const int hi1:1690*80;
    const int repeatHi:2250*80;
};

#endif
