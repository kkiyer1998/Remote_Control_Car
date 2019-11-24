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

//turns the car on or off
void power()
{
    car.power = car.power == 0 ? 1 : 0;
    if(!car.power)
        halt();
}

//this increase the speed of the car by 5%
void increaseSpeed()
{
    if(car.direction == FORWARD)
    {
        int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
        int newSpeed = averageSpeed + 5 > 100 ? 100 : averageSpeed + 5;
        car.leftSpeed = car.rightSpeed =newSpeed;
    }
    else
        decreaseSpeed()


}

//this decreses the speed of the car by 5%
//might change the direction of the car
void decreaseSpeed()
{
    int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
    if(averageSpeed - 5 < 0)
    {
        changeDirection();
        car.leftSpeed = car.rightSpeed = averageSpeed + 5;
        return;
    }
    if(car.direction == FORWARD)
        car.leftSpeed = car.rightSpeed = averageSpeed - 5;
    else
        increaseSpeed();
}

//This function changes the direction of the car
//NEED TO IMPLEMENT THE ACTUAL IMPLEMENTATION
void changeDirection()
{
    car.direction = car.direction == FORWARD ? BACKWARD : FORWARD;
}

//This function pulls the car out of turn mode
void straighten()
{
    return (car.leftSpeed+car.rightSpeed)/2;
}

//turns the car left
//the speed is messed up after this, need to re-increase the speed
void turnLeft()
{
    car.leftSpeed = 5;
    car.rightSpeed = 20;
}

//turns the car right
//the speed is messed up after this, need to re-increase the speed
void turnRight()
{
    car.rightSpeed = 5;
    car.leftSpeed = 20;
}

//stops the car
void halt()
{
    car.leftSpeed = 0;
    car.rightSpeed = 0;
}

//changes the duty cycle of the timers
// LOOK AT THIS ONE WHILE SETTING UP THE MOTOR DRIVER
void commitChange()
{
    TIMER0_TAMTCHR_R = (car.leftSpeed*TIMER0_TAILR_R)/100;
    TIMER0_TBMTCHR_R = (car.rightSpeed*TIMER0_TAILR_R)/100;
}
