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

void power()
{
    car.power = car.power == 0 ? 1 : 0;
    if(!car.power)
        halt();
}

void increaseSpeed()
{
    int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
    int newSpeed = averageSpeed + 5 > 100 ? 100 : averageSpeed + 5;
    car.leftSpeed = car.rightSpeed =newSpeed;
}

void decreaseSpeed()
{
    int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
    if(averageSpeed - 5 < 0)
    {
        changeDirection();
        return;
    }
    car.leftSpeed = car.rightSpeed = averageSpeed - 5;
}

void changeDirection()
{
    car.direction = car.direction == FORWARD ? BACKWARD : FORWARD;
}

void straighten()
{
    return (car.leftSpeed+car.rightSpeed)/2;
}

void turnLeft()
{
    car.leftSpeed = 5;
    car.rightSpeed = 20;
}

void turnRight()
{
    car.rightSpeed = 5;
    car.leftSpeed = 20;
}

void halt()
{
    car.leftSpeed = 0;
    car.rightSpeed = 0;
}

void commitChange()
{
    TIMER0_TAMTCHR_R = (car.leftSpeed*TIMER0_TAILR_R)/100;
    TIMER0_TBMTCHR_R = (car.rightSpeed*TIMER0_TAILR_R)/100;
}
