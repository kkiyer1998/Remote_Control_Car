#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"

#define TOLERANCE 20

int  done = 0 , period = 0, first = 0;

//reads a bit
int bitRead();

enum button
{
    POWER,
    A,
    B,
    C,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CIRCLE
};

enum state
{
    START_LOW,
    START_HIGH,
    LOW,
    HI0,
    HI1,
    NONE
};

int curState = NONE;





void setup();

void PLLInit();

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

void read ()
{
    while(done == 0);
    done = 0;
    int b = bitRead();
    if(b == NONE || b == START_LOW)
    {
        reset();
        curState = b;
        return;
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
            if(b == HI0)
                add(0);
            else if(b == HI1)
                add(1);
            curState = START_HIGH;
            return;
    }
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

    while (1)
    {
        if (complete())
        {
            int local = getButton();
            GPIO_PORTF_DATA_R = local;
            reset();
        }
        read();
    }
    return 0;
}


void Timer0A_Handler(void)
{
    period = (first - TIMER0_TAR_R) & 0x00FFFFFF;
    TIMER0_ICR_R = 0x00000004; //acknowledgement
    first = TIMER0_TAR_R ;
    done = 1;
}


#define TOLERANCE 20


int in_range(int quantity, int expected)
{
    return quantity >= (expected* (100-TOLERANCE))/100
                && quantity <= (expected* (100+TOLERANCE))/100;
}

int bitRead(struct Signal)
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
    return NONE;
}
