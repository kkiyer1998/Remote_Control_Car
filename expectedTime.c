#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"

#include "expectedTime.h"
#include "enums.h"


#define TOLERANCE 20

struct expectedTime et;

void setEnumTime()
{
    et.startHigh=4500*80;
    et.startLow=9000*80;
    et.low=560*80;
    et.hi0=560*80;
    et.hi1=1690*80;
    et.repeatHi=2250*80;
}

int in_range(int quantity, int expected)
{
    return quantity >= (expected* (100-TOLERANCE))/100
                && quantity <= (expected* (100+TOLERANCE))/100;
}

//the IR signal is inverted
int bitRead(int period, int curState)
{
    if(in_range(period,et.startLow))
        return START_LOW;
    if(in_range(period,et.startHigh))
        return START_HIGH;
    if(in_range(period,et.low) && curState != LOW)
        return LOW;
    if(in_range(period,et.hi0))
        return HI0;
    if(in_range(period,et.hi1))
        return HI1;
    if(in_range(period,et.repeatHi))
        return REPEAT_HI;
    return NONE;
}
