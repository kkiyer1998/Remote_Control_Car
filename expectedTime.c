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

int in_range(int quantity)
{
    return quantity >= (expected* (100-TOLERANCE))/100
                && quantity <= (expected* (100+TOLERANCE))/100;
}

//the IR signal is inverted
int bitRead(int period)
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
