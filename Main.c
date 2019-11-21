#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <sysctl.h>
#include "timer.h"
#include "Serial.h"

#define TOLERANCE 20

int  done = 0 , period = 0, first = 0;

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

struct expectedTime
{
    int startLow;
    int startHigh;
    int low;
    int hi0;
    int hi1;
}et;


int in_range(int quantity, int expected); //checks whether in toleable range
//reads a bit
int bitRead();

struct Signal
{
    int arr[32];
    int index;
}signal;

//resets the signal
void reset();

//checks if the signal is complete
int complete();

//adds a bit to the signal
void add(int b);

//requires !complete()
//converts a Signal to integer
unsigned int convert();


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

    et.startLow = 9000*80;
    et.startHigh = 4500*80;
    et.low = 560*80,
    et.hi0 = 560*80;
    et.hi1 = 1690*80;
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


int in_range(int quantity, int expected)
{
    return quantity >= (expected* (100-TOLERANCE))/100
                && quantity <= (expected* (100+TOLERANCE))/100;
}

int bitRead()
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


void reset()
{
    signal.index = 0;
    int i;
    for(i=0; i < 33; i++)
        signal.arr[i] = 0;
}

int complete()
{
    return signal.index == 32;
}

void add(int b)
{
    if(complete())
        return ;
    signal.arr[signal.index] = b;
    signal.index++;
}

//requires !complete()
//converts a Signal to integer
unsigned int convert()
{
    int result = 0, i =0;
    for(; i < 32; i++)
    {
        result = (result << 1) +( signal.arr[i] & 1);
    }
    return result;
}

void Timer0A_Handler(void)
{
    period = (first - TIMER0_TAR_R) & 0x00FFFFFF;
    TIMER0_ICR_R = 0x00000004; //acknowledgement
    first = TIMER0_TAR_R ;
    done = 1;
}
