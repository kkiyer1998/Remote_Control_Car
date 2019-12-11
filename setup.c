#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>

#include "timer.h"
#include "Serial.h"
#include "setup.h"
#include "expectedTime.h"
#include "signal.h"


void PLLInit()
{
    SYSCTL_RCC2_R |= 0x80000000;
    SYSCTL_RCC2_R |= 0x00000800;
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) + 0x00000540;
    SYSCTL_RCC2_R &= ~0x00000070;
    SYSCTL_RCC2_R &= ~0x00002000;
    SYSCTL_RCC2_R |= 0x40000000;
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (4 << 22);
    while ((SYSCTL_RIS_R & 0x00000040) == 0)
    {
    };
    SYSCTL_RCC2_R &= ~0x00000800;
}

// FOR THE MOTOR: PWM
//uses PB6 and PB7
void Timer0Setup()
{
    SYSCTL_RCGCTIMER_R |= 0X01; //ACTIVATE TIMER 0
    SYSCTL_RCGCGPIO_R |= 0X02; //ACTIVATE PORT B
    while((SYSCTL_PRGPIO_R&0X02) == 0){};
    GPIO_PORTB_AFSEL_R |= 0XC0; //ENABLE ALTERATE FUNCTIONALITY ON PB6 and PB7
    GPIO_PORTB_DEN_R |= 0XC0; //ENABLE DIGITAL I/O ON PB6 and PB7
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0X00FFFFFF) + 0X77000000;
    TIMER0_CTL_R &= ~0x0101; //DISABLE TIMER0 DURING SETUP
    TIMER0_CFG_R = 0X00000004; //CONFIGURE FOR 16-BIT TIMER MODE

    TIMER0_TAMR_R = 0X0000000A; //PWM AND PERIODIC MODE
    TIMER0_TAILR_R = 0X1F40; // PERIOD = 100microsec
    TIMER0_TAMATCHR_R = TIMER0_TAILR_R-10; // INITIALIZE DUTY CYCLE = 0%
    //TIMER0_TAPWML = 0x0;


    TIMER0_TBMR_R = 0X0000000A; //PWM AND PERIODIC MODE
    TIMER0_TBILR_R = 0X1F40; // PERIOD = 100microsec
    TIMER0_TBMATCHR_R = TIMER0_TBILR_R-10; // INITIALIZE DUTY CYCLE = 0%
    //TIMER0_TBPWML = 0x0;

    TIMER0_CTL_R |= 0X00000101; //ENABLE TIMER0
}

//timer for IR sensor : setup for input capture
//uses PB0
void Timer2Setup()
{
    //TURN ON THE CLOCK FOR PORT B
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x04; //activate timer 2

    GPIO_PORTB_DEN_R |= 0X01; // ENABLE DIGITAL I/O ON PB0
    GPIO_PORTB_AFSEL_R |= 0X01; //ENABLE ALTERNATE FUNCTIONALITY ON PB0
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFFF0) + 0x00000007;

    TIMER2_CTL_R &= ~0x00000001; // Disable timer 2 for configuration
    TIMER2_CFG_R = 0x00000004; // Configure for 16-bit capture mode
    TIMER2_TAMR_R = 0x00000007; //CONFIGURE FOR INPUT CAPTURE MODE

    TIMER2_CTL_R = 0X0000000C; // CONFIGURE FOR BOTH RISING AND FALLING EDGE
    TIMER2_TAILR_R = 0x0000FFFF; // Start value for count down
    TIMER2_TAPR_R = 0xFF; // Activate pre-scale

    TIMER2_IMR_R |= 0x00000004; // Enable Input capture interrupts

    TIMER2_ICR_R = 0x00000004; // Clear Timer2A capture match flag
    TIMER2_CTL_R |= 0x00000001; // Timer 2A

    NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF) | 0x40000000; //TIMER 2A PRIORITY 2
    NVIC_EN0_R = 1<<23;
}

//test port
void portFSetup()
{
    //TURN ON THE CLOCK FOR PORT F
    SYSCTL_RCGCGPIO_R |= 0x20;

    GPIO_PORTF_CR_R = 0XFF;

    //SELECT ANALOG FUNCTIONALITY
    //0 MEANS DISABLE
    GPIO_PORTF_AMSEL_R = 0X00;

    //CONFIGURE PCTL REGISTER
    GPIO_PORTF_PCTL_R = 0X00000000;

    //SET INPUT OR OUTPUT : 0 IS INPUT AND 1 IS OUTPUT
    GPIO_PORTF_DIR_R = 0XFF;

    //KEEP ALTERNATE FUNCTIONALITY TO 0
    GPIO_PORTF_AFSEL_R = 0X00;

    //ENABLE DIGITAL PORTS
    GPIO_PORTF_DEN_R = 0XFF;

    GPIO_PORTF_PUR_R = 0XFF;
}


void setup()
{
    Timer0Setup();
    Timer2Setup();
    portFSetup();
    setEnumTime();
    carSetup();
    reset(); //IR Signal
    SetupSerial();
    SystickInit();
}
