#include <stdio.h>
#include <at89lp51rd2.h>
#include <stdlib.h>

#define DISTANCE 0B_1111_1111

int distance = DISTANCE; 

int send_to_Queue(void)
{
	int parkFlag = 0;
	int leftTurn = 0;
	int rightTurn = 0;
	int forward = 0;
	int backward = 0;
	int reverse_LIGHT = 0;
	int head_LIGHT = 0;
	int left_LIGHT = 0;
	int right_LIGHT = 0;
	int super_fast_TURBO = 0;
	
	while(1)   //constantly checks for buttons
	{
	
	while(PARKING_BUTTON == 0); // set parking button
	while(PARKING_BUTTON == 1) {
		if(parkFlag == 0) {
			parkFlag = 1;
	  		addtoqueue(parkFlag);
	  	}
	}
	parkFlag = 0;
	
	while(LEFT_TURN == 0);	// set left turn button
	while(LEFT_TURN == 1) {
		if(leftTurn == 0) {
			leftTurn = 4;
			addtoqueue(left_LIGHT)
	  		addtoqueue(leftTurn);
	  	}
	}
	leftTurn = 0;
	
	while(RIGHT_TURN == 0); // set right turn button
	while(RIGHT_TURN == 1) {
		if(rightTurn == 0) {
			rightTurn = 6;
			addtoqueue(right_LIGHT);
	  		addtoqueue(rightTurn);
	  	}
	}
	parkFlag = 0;
	
	
	
	while(FORWARD == 0); // set forward button 
	while(FORWARD == 1) {
		if(forward == 0) {
			forward = 8;
			addtoqueue(head_LIGHT);
	  		addtoqueue(forward);
	  	}
	}
	forward = 0;
	
	
	
	while(BACKWARD == 0); // set backwards button
	while(BACKWARD == 1) {
		if(backward == 0) {
			backward = 2;
			addtoqueue(reverse_LIGHT);
	  		addtoqueue(backward);
	  	}
	}
	backward = 0;
	}	
}
