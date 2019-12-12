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

#define FORWARD (0)
#define BACKWARD (1)

struct Car car;

//changes the duty cycle of the timers
// LOOK AT THIS ONE WHILE SETTING UP THE MOTOR DRIVER
void commitChange()
{
    TIMER0_TAMATCHR_R = ((long)(100-car.leftSpeed)*(TIMER0_TAILR_R-10))/100;
    TIMER0_TBMATCHR_R =  ((long)(100-car.rightSpeed)*(TIMER0_TBILR_R-10))/100;
}


void carSetup()
{
    car.power = 0;
    car.direction = FORWARD;
    GPIO_PORTF_DATA_R = 1;
    halt();
    commitChange();
}

//turns the car on or off
void power()
{
    car.power = car.power == 0 ? 1 : 0;
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x10)| (car.power << 4);
    halt();
    car.direction = FORWARD;
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x5) | 4;
}

//this increase the speed of the car by 5%
void increaseSpeed()
{
    if(car.power == 0)
        return;

    int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
    int newSpeed = averageSpeed + 20 > 100 ? 100 : averageSpeed + 20;
    car.leftSpeed = car.rightSpeed = newSpeed;
}

//this decreses the speed of the car by 5%
//might change the direction of the car
void decreaseSpeed()
{
    if(car.power == 0)
        return;
    int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
    if(car.direction == FORWARD)
    {
        if (averageSpeed <= 20)
            changeDirection();
        else
            car.leftSpeed = car.rightSpeed = averageSpeed - 20;
    }
    else
    {
        if (averageSpeed <= 20)
           changeDirection();
        else
           car.leftSpeed = car.rightSpeed =averageSpeed - 20;
    }
}

//This function changes the direction of the car
//NEED TO IMPLEMENT THE ACTUAL IMPLEMENTATION
void changeDirection()
{
    if(car.power == 0)
            return;
    car.direction = car.direction == FORWARD ? BACKWARD : FORWARD;
    if(car.direction == FORWARD)
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0xF) | 0x4;
    else
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0xF) |0x2;
    //halt();

}


//turns the car left
//the speed is messed up after this, need to re-increase the speed
void turnLeft()
{
    if(car.power == 0)
            return;
    car.leftSpeed = 20;
    car.rightSpeed = 60;
}

//turns the car right
//the speed is messed up after this, need to re-increase the speed
void turnRight()
{
    if(car.power == 0)
            return;
    car.rightSpeed = 20;
    car.leftSpeed = 60;
}

//stops the car
void halt()
{

    car.leftSpeed = 0;
    car.rightSpeed = 0;
}

