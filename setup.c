#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>

#include "timer.h"
#include "Serial.h"
#include "setup.h"
#include "expectedTime.h"
#include "signal.h"


void setup()
{
    //TURN ON THE CLOCK
    SYSCTL_RCGCGPIO_R |= 0x20 | 0x02;

    SYSCTL_RCGCTIMER_R |= 0x01; //activate timer 0
    GPIO_PORTB_DIR_R &= ~0X04;
    GPIO_PORTB_AFSEL_R |= 0X40;
    GPIO_PORTB_DEN_R |= 0X40;

    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xF0FFFFFF) + 0x07000000;
    TIMER0_CTL_R &= ~0x00000001; // Disable timer 0 for configuration – more on this in a bit
    TIMER0_CFG_R = 0x00000004; // Configure for 16-bit capture mode
    TIMER0_TAMR_R = 0x00000007;

    TIMER0_CTL_R = 0x0000000C; // configure for rising edge
    TIMER0_TAILR_R = 0x0000FFFF; // Start value for count down
    TIMER0_TAPR_R = 0xFF; // Activate pre-scale
    TIMER0_IMR_R |= 0x00000004; // Enable Input capture interrupts

    TIMER0_ICR_R = 0x00000004; // Clear Timer0A capture match flag
    TIMER0_CTL_R |= 0x00000001; // Timer 0A 24-bit
    NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF) | 0x40000000;
    NVIC_EN0_R = 1<<19;


    //UNLOCK REGISTER
    // GPIO_PORTB_LOCK_R = 0X4C4F434B;
    GPIO_PORTF_LOCK_R = 0X4C4F434B;

    // //ENABLE BITS IN THE COMMIT REGISTER
    // // GPIO_PORTB_CR_R = 0XFF;
    GPIO_PORTF_CR_R = 0XFF;

    // //SELECT ANALOG FUNCTIONALITY
    // //0 MEANS DISABLE
    // // GPIO_PORTB_AMSEL_R = 0X00;
    GPIO_PORTF_AMSEL_R = 0X00;

    // //CONFIGURE PCTL REGISTER
    // // GPIO_PORTB_PCTL_R = 0X00000000;
    GPIO_PORTF_PCTL_R = 0X00000000;

    // //SET INPUT OR OUTPUT : 0 IS INPUT AND 1 IS OUTPUT

    GPIO_PORTF_DIR_R = 0XF;

    // //KEEP ALTERNATE FUNCTIONALITY TO 0
    GPIO_PORTF_AFSEL_R = 0X00;

    //ENABLE DIGITAL PORTS

    GPIO_PORTF_DEN_R = 0X0F;

    //PULLUP RESISTORS : USED TO ENABLE TRANSISTORS : PAGE 8 LECTURE 4
    // GPIO_PORTB_PUR_R = 0X0F; //all ports are enabled
    GPIO_PORTF_PUR_R = 0X0F; //all ports are enabled

    signal.index = 0;
}

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
