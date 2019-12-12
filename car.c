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


int voltage;
void Timer3A_Handler(void) {
    ADC0_ISC_R = 0x008;
    voltage = ADC0_SSFIFO3_R;
    //SerialWriteInt(voltage);
}

//changes the duty cycle of the timers
// LOOK AT THIS ONE WHILE SETTING UP THE MOTOR DRIVER
void commitChange()
{
    TIMER0_TAMATCHR_R = ((100-car.leftSpeed)*(TIMER0_TAILR_R-10))/100;
    TIMER0_TBMATCHR_R =  ((100-car.rightSpeed)*(TIMER0_TBILR_R-10))/100;
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
    int local = car.direction;
    car.direction = FORWARD;
    GPIO_PORTF_DATA_R |= 1;
}

//this increase the speed of the car by 5%
void increaseSpeed()
{
    if(car.power == 0)
        return;
    if(car.direction == FORWARD)
    {
        int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
        int newSpeed = averageSpeed + 10 > 100 ? 100 : averageSpeed + 10;
        car.leftSpeed = car.rightSpeed =newSpeed;
    }
    else
    {
        int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
        int newSpeed = averageSpeed - 10 < 0 ? 0 : averageSpeed - 10;
        if (newSpeed == 0) {
            changeDirection();
        }
        car.leftSpeed = car.rightSpeed = newSpeed;
    }
}

//this decreses the speed of the car by 5%
//might change the direction of the car
void decreaseSpeed()
{
    if(car.power == 0)
        return;
    if(car.direction == FORWARD)
    {
        int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
        int newSpeed = averageSpeed - 10 < 0 ? 0 : averageSpeed - 10;
        if (newSpeed == 0) {
            changeDirection();
        }
        int local = car.direction;
        car.leftSpeed = car.rightSpeed =newSpeed;
    }
    else
    {
        int averageSpeed = (car.leftSpeed+car.rightSpeed)/2;
        int newSpeed = averageSpeed + 10 > 100 ? 100 : averageSpeed + 10;
        car.leftSpeed = car.rightSpeed = newSpeed;
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
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x3) | 1;
    else
        GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x3) |0x2;
    //halt();

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
    if(car.power == 0)
            return;
    car.leftSpeed = 5;
    car.rightSpeed = 20;
}

//turns the car right
//the speed is messed up after this, need to re-increase the speed
void turnRight()
{
    if(car.power == 0)
            return;
    car.rightSpeed = 5;
    car.leftSpeed = 20;
}

//stops the car
void halt()
{
    car.leftSpeed = 0;
    car.rightSpeed = 0;
    car.direction = FORWARD;
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x3) | 1;
}

