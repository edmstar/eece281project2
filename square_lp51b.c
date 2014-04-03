#include <stdio.h>
#include <stdlib.h>
#include <at89lp51rd2.h>

#include "queue.h"

#include "queue.c"

// ~C51~ 
 
#define CLK 22118400L
#define BAUD 115200L
#define BRG_VAL (0x100-(CLK/(32L*BAUD)))

//We want timer 0 to interrupt every 100 microseconds ((1/10000Hz)=100 us)
#define FREQ 30630L
#define TIMER0_RELOAD_VALUE (65536L-(CLK/(12L*FREQ)))

#define STATE_KEEP_50_AWAY 1
#define STATE_KEEP_50_AWAY 1
#define STATE_KEEP_50_AWAY 1

#define DISTANCE_BYTE 0B_1111_1111
#define NOTHING_BYTE 0B_0000_0000

//These variables are used in the ISR
volatile unsigned char pwmcount;
volatile unsigned char pwm1;
volatile txon = 0;
volatile state = 0;
volatile struct Queue* q;
volatile struct QueueItem* item;
	
#include "transmitter.c"

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0 &= 0B_1111_1100; P2M1 |= 0B_0000_0011;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
    
    // Initialize the serial port and baud rate generator
    PCON|=0x80;
	SCON = 0x52;
    BDRCON=0;
    BRL=BRG_VAL;
    BDRCON=BRR|TBCK|RBCK|SPD;
	
	// Initialize timer 0 for ISR 'pwmcounter()' below
	TR0=0; // Stop timer 0
	TMOD=0x01; // 16-bit timer
	// Use the autoreload feature available in the AT89LP51RB2
	// WARNING: There was an error in at89lp51rd2.h that prevents the
	// autoreload feature to work.  Please download a newer at89lp51rd2.h
	// file and copy it to the crosside\call51\include folder.
	TH0=RH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=RL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0 (bit 4 in TCON)
	ET0=1; // Enable timer 0 interrupt
	EA=1;  // Enable global interrupts
	
	pwmcount=0;
    
    return 0;
}

// Interrupt 1 is for timer 0.  This function is executed every time
// timer 0 overflows: 100 us.
void pwmcounter (void) interrupt 1
{
	if (txon == 1) {
		P2_0=(P2_0==1)?0:1;
		if (P2_0 == 1){
			P2_1 = 0;
		}
		else {
			P2_1 = 1;
		}
	} else {
		P2_0 = 1;
		P2_1 = 1;
	}
}

void main (void)
{

	int flagTransmitter = 0;
	
	q = newQueue();
	append(q, newItem(q, 'c'));
	append(q, newItem(q, 'd'));
	
	while(1) {
		if (flagTransmitter == 0) flagTransmitter = 1;
		else flagTransmitter = 0;
		
		if (flagTransmitter == 0) {
			if (q->count > 0) {
				item = pop(q);
				tx_byte(item->value);
			} else {
				tx_byte(NOTHING_BYTE);
			}
		} else {
			tx_byte(DISTANCE_BYTE);
		}
		wait_delay();
	}
	
	pwm1=50; //50% duty cycle wave at 100Hz
	printf( "\nPlease check P1.0 with the oscilloscope.\n" );
}

