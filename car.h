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

#define FORWARD (0)
#define BACKWARD (1)

struct Car
{
    unsigned int leftSpeed; //speeds are in %
    unsigned int rightSpeed; //speeds are in %
    int direction;
    int power;
};

void carSetup();

void power();

void increaseSpeed();

void decreaseSpeed();

void changeDirection();

void straighten();

void turnLeft();

void turnRight();

void halt();

