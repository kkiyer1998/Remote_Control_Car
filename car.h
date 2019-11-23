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
    unsigned int leftSpeed : 0; //speeds are in %
    unsigned int rightSpeed : 0; //speeds are in %
    int direction : FORWARD;
    int power : 0;
};

void power();

void increaseSpeed();

void decreaseSpeed();

void changeDirection();

void straighten();

void turnLeft();

void turnLeft();

