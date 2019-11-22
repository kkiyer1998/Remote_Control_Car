#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"
#include "enums.h"
#include "expectedTime.h"
#include "setup.h"
#include "signal.h"

#define TOLERANCE 20

int  period = 0, first = 0;

int curState = NONE;

int getButton()
{
    switch(convert())
    {
        case 0x00FF629D : return POWER;
        case 0x00FF22DD : return A;
        case 0x00FF02FD : return B;
        case 0x00FFC23D : return C;
        case 0x00FF9867 : return UP;
        case 0x00FF38C7 : return DOWN;
        case 0x00FF30CF : return LEFT;
        case 0x00FF7A85 : return RIGHT;
        case 0x00FF18E7 : return CIRCLE;
    }
}

void read (int period)
{
    int b = bitRead(period);
    if(b == NONE)
    {
        reset();
        curState = b;
        return;
    }
    else if (b == START_LOW)
    {
        curState = b;
        return;
    }
    else if(curState == START_LOW && b == REPEAT_HI)
    {
        curState = REPEAT_HI;
        return;
    }
    else if(curState == REPEAT_HI && b == LOW)
    {
        curState = NONE;
    }
    else if(b == START_HIGH && curState == START_LOW)
    {
        curState = START_HIGH;
        return;
    }
    else if(curState == START_HIGH && b==LOW)
    {
        curState = LOW;
        return;
    }
    else if(curState == LOW && (b == HI0 || b == HI1))
    {
            if(complete())
                reset();
            if(b == HI0)
                add(0);
            else if(b == HI1)
                add(1);
            curState = START_HIGH;
            return;
    }
    reset();
    curState = NONE;
}



/*
        * main.c
        */
int main(void)
{
    PLLInit();
    setup();
    SetupSerial();

    while (1);
    // {
        // if (complete())
        // {
        //     int local = getButton();
        //     GPIO_PORTF_DATA_R = local;
        //     reset();
        // }
    // }
    return 0;
}


//this is a handler for the IR receiver
//This handler is called for every edge: rising or falling
//This function reads the signal using read()
//This function should be fast enough to detect every edge
void Timer0A_Handler(void)
{
    period = (first - TIMER0_TAR_R) & 0x00FFFFFF;
    read(period);
    first = TIMER0_TAR_R ;
    TIMER0_ICR_R = 0x00000004; //acknowledgement
}


