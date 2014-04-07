#include <stdio.h>
#include <stdlib.h>
#include <at89lp51rd2.h>

#include "queue.h"
#include "queue.c"


#define CLK 22118400L
#define BAUD 115200L
#define BRG_VAL (0x100-(CLK/(32L*BAUD)))
#define VCC 4.84

//We want timer 0 to interrupt every 100 microseconds ((1/10000Hz)=100 us)
#define FREQ 1000L
#define TIMER0_RELOAD_VALUE (65536L-(CLK/(12L*FREQ)))

#define DISTANCE_BYTE 0B_0000_0011
#define NOTHING_BYTE 0B_0000_0000

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d\n"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0) 

//Percentage constants
#define TH_PERCENT_TURN 0.2000

#define TH_PERCENT_BF_1 0.9000
#define TH_PERCENT_BF_2 0.6000
#define TH_PERCENT_BF_3 0.2500
#define TH_PERCENT_BF_4 0.2000
#define TH_PERCENT_BF_5 0.1500

//distance constants
#define LEVEL_1 0.015
#define LEVEL_2 0.100
#define LEVEL_3 0.400
#define LEVEL_4 0.750
#define LEVEL_5 1.000
  
//command constants
#define PARALLEL_PARK 'b'   
#define SPIN_O_RAMA 'S'
#define DO_NOTHING 'N'
#define LEVEL_UP 'U'
#define LEVEL_DOWN 'D'

#define MOVE_UP 'w'
#define MOVE_DOWN 's'
#define MOVE_RIGHT 'd'
#define MOVE_LEFT 'a'

#define LEVEL 'L'

//movement constants
#define RIGHT 6
#define LEFT 4
#define FOR 2
#define BACK 8
#define STOP 0
  
volatile float minimum_logic_voltage = 7e-3;
volatile float volt_right = 0;
volatile float volt_left = 0;
volatile unsigned char pwmcount, pwm1, pwm2, pwm3, pwm4;
volatile struct Queue* q;
volatile zeroCount = 0;

float *levels;//[5] = {LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5};
float *thresholds;//[4];// = {TH_PERCENT_BF_1, TH_PERCENT_BF_2, TH_PERCENT_BF_3, TH_PERCENT_BF_4, TH_PERCENT_BF_5};

unsigned int level;
char state;

#include "functions.c"

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0=0;	P2M1=0;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
    
    // Instead of using a timer to generate the clock for the serial
    // port, use the built-in baud rate generator.
    PCON|=0x80;
	SCON = 0x52;
    BDRCON=0;
    BRL=BRG_VAL;
    BDRCON=BRR|TBCK|RBCK|SPD;
    
    TR0=0; 
	TMOD=0x01;

	TH0=RH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=RL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1;
	ET0=1; 
	EA=0;
	
    return 0;
}

void SPIWrite(unsigned char value)
{
	SPSTA&=(~SPIF); // Clear the SPIF flag in SPSTA
	SPDAT=value;
	while((SPSTA & SPIF)!=SPIF); //Wait for transmission to end
}

unsigned int GetADC(unsigned char channel)
{
	unsigned int adc;

	// initialize the SPI port to read the MCP3004 ADC attached to it.
	SPCON&=(~SPEN); // Disable SPI
	SPCON=MSTR|CPOL|CPHA|SPR1|SPR0|SSDIS;
	SPCON|=SPEN; // Enable SPI
	
	P1_4=0; // Activate the MCP3004 ADC.
	SPIWrite(channel|0x18);	// Send start bit, single/diff* bit, D2, D1, and D0 bits.
	for(adc=0; adc<10; adc++); // Wait for S/H to setup
	SPIWrite(0x55); // Read bits 9 down to 4
	adc=((SPDAT&0x3f)*0x100);
	SPIWrite(0x55);// Read bits 3 down to 0
	P1_4=1; // Deactivate the MCP3004 ADC.
	adc+=(SPDAT&0xf0); // SPDR contains the low part of the result. 
	adc>>=4;
		
	return adc;
}

float voltage (unsigned char channel)
{
	return ( (GetADC(channel)*VCC)/1023.0 ); // VCC=4.84V (measured)
}

unsigned char rx_byte ( float min )
{
	unsigned char j;
	unsigned char val = 0;
	float v;
	int c;

	wait_one_and_half_bit_time();
	
	for(j=0; j<8; j++) {
		v = voltage(0);
		if (j == 0) {
			volt_right = v;
			volt_left = voltage(1);
		}
		c = (v>min);
		P2_0 = c;
		val |= (c)?(0x01<<j):0x00;
		wait_bit_time();
	}
	return val;
}

void pwmcounter (void) interrupt 1
{
	
	float f;
	volatile char b;
	struct QueueItem* item;
	
	EA = 0;

	f = voltage(0);
	//printf("%f %f", f, minimum_logic_voltage);
	
	if (f > minimum_logic_voltage) {
	
		P2_0 = 0;
		b = rx_byte((f*0.25));
		
		if (b != NOTHING_BYTE && b != DISTANCE_BYTE) {
			item = newItem(b);
			append(q, item);
			zeroCount = 0;
		} else if (b == NOTHING_BYTE) {
			zeroCount++;
		} else {
			zeroCount = 0;
		}

		//printf ("Bits - "BYTETOBINARYPATTERN, BYTETOBINARY(b));
		//printf ("%c - %f\n", b, f);
		printf ("%c %f - %f\n", b, volt_left, volt_right);
	}

	if(++pwmcount>10) {
		pwmcount=0;
	}

	P1_0=(pwm1>pwmcount)?1:0;
	P1_1 = (pwm2 > pwmcount)? 1:0;
	P0_0=(pwm3>pwmcount)?1:0;
	P0_1 = (pwm4 > pwmcount)? 1:0;

	
	EA = 1;
}

void callCommand(char command) {
	//printf("Command: %c\n", command);
	switch(command) {
		case PARALLEL_PARK:
			state = command;
			break;
		case SPIN_O_RAMA:
			state = command;
			break;
		case LEVEL_UP:
			state = LEVEL;
			if (level < 4) level++;
			else level = 4;
			break;
		case LEVEL_DOWN:
			state = LEVEL;
			if (level > 0) level--;
			else level = 0;
			break;			
		case DO_NOTHING:
			state = command;
			break;
	}
}
		

void main(void) {

	struct QueueItem* item;
	float f;
	
	levels = (float *) malloc(sizeof(float)*5);
	thresholds = (float *) malloc(sizeof(float)*5);
	
	levels[0] = LEVEL_1;
	levels[1] = LEVEL_2;
	levels[2] = LEVEL_3;
	levels[3] = LEVEL_4;
	levels[4] = LEVEL_5;
	
	thresholds[0] = TH_PERCENT_BF_1;
	thresholds[1] = TH_PERCENT_BF_2;
	thresholds[2] = TH_PERCENT_BF_3;
	thresholds[3] = TH_PERCENT_BF_4;
	thresholds[4] = TH_PERCENT_BF_5;
	
	
	q = newQueue();
	
	pwm1 = 0;
	pwm2 = 0;
	pwm3 = 0;
	pwm4 = 0;
	
	level = 0;
	state = 0;
	
	f = voltage(0);
	
	while (f < minimum_logic_voltage) {
		f = voltage(0);
	}

	EA = 1;
	
	while(1) {
	
		if (q->count > 0) {
			item = pop(q);
			//printf("%d\n", q->count);
			callCommand(item->value);
			free(&(item->value));
		}
		
		
		if (zeroCount < 5) {
		
			switch(state) {
			
				case LEVEL:
					level_function();
					break;
					
				case PARALLEL_PARK:
					parallel_park();
					callCommand(DO_NOTHING);
					break;
					
				case SPIN_O_RAMA:
					spin_o_rama();
					callCommand(DO_NOTHING);
					break;
					
				case DO_NOTHING:
				default:
					stop();
					break;
			}
				
		} else {
			stop();
		}
		
	}
	
}
