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
#include "car.h"

#define TOLERANCE 20

int  period = 0, first = 0;

int curState = NONE;

extern struct expectedTime et;
extern struct Car car;

//gets which button was pressed
int getButton()
{
    switch(convert())
    {
        case 0x00FF629D : return POWER;
        case 0x00FF9867 : return UP;
        case 0x00FF38C7 : return DOWN;
        case 0x00FF30CF : return LEFT;
        case 0x00FF7A85 : return RIGHT;
        case 0x00FF18E7 : return CIRCLE;
    }
    return -1;
}


//state change function
//populates the signal
void read (int period)
{
    int local = period;
    int b = bitRead(period,curState);
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

    while (1);
    return 0;
}

//performs the necessary action on the car
void action(int button)
{
    int local = button;
    return SerialWriteInt(button);
    switch (button)
    {
        case POWER: return power();
        case UP: return car.direction == FORWARD ? increaseSpeed() : decreaseSpeed();
        case DOWN: return car.direction == BACKWARD ? increaseSpeed() : decreaseSpeed();
        case LEFT: return turnLeft();
        case RIGHT: return turnRight();
        case CIRCLE: return halt();
    }
}


//this is a handler for the IR receiver
//This handler is called for every edge: rising or falling
//This function reads the signal using read()
//This function should be fast enough to detect every edge
void Timer2A_Handler(void)
{

    period = (first-TIMER2_TAR_R) & 0x00FFFFFF;
    //SerialWriteInt(first);
    //SerialWriteInt(TIMER2_TAR_R);

    int local = period;
    read(period);
    local = curState;
    if(complete())
    {
        int button = getButton();
        action(button);
    }
    first = TIMER2_TAR_R ;
    //SerialWriteInt(first);
    //SerialWriteInt(0);
    TIMER2_ICR_R = 0x00000004; //acknowledgement
}


