void wait_bit_time(void) {
	_asm
		    mov R1, #25
		L1: mov R0, #255
		L0: djnz R0, L0
			djnz R1, L1
			ret
	_endasm;
}

void wait_one_and_half_bit_time(void) {
	_asm
		    mov R1, #43
		L4: mov R0, #253
		L5: djnz R0, L5
			djnz R1, L4
			ret
	_endasm;
}

void wait_delay(void) {
	_asm
		    mov R1, #255
		L2: mov R0, #255
		L3: djnz R0, L3
			djnz R1, L2
			ret
	_endasm;
}

float max( float voltage_right, float voltage_left){
	return (voltage_right>voltage_left) ? voltage_right : voltage_left;
}

float min( float voltage_right, float voltage_left){
	return (voltage_right<voltage_left) ? voltage_right : voltage_left;
}

int movement_logic_RL (float Vright, float Vleft, float threshold_RL) {

	int state_RL=0;
	
	if((max(Vright, Vleft) - min(Vright, Vleft)) > threshold_RL)
	{
		if(Vleft > Vright) {
			state_RL = RIGHT;
		} else if(Vright > Vleft) {
			state_RL = LEFT;
		} else {
			state_RL = 0;
		}
	} else {
		state_RL = 0;
	}
	
	return state_RL;
}

int movement_logic_BF (float Vright, float Vleft, float threshold_BF, float levelV) {
		
	int state_BF=0;
	float range_right = (Vright - levelV);
	float range_left = (Vleft - levelV);
	
	threshold_BF *= 0.5;
		
	if( ( range_right > threshold_BF ) || ( range_left > threshold_BF ) ) {
		state_BF = BACK;	
	} else if( ( -range_right > threshold_BF ) || ( -range_left > threshold_BF ) ) {
		state_BF = FOR;
	} else {
		state_BF = STOP;
	}
		
	return state_BF;
}

void Turn_right(void)
{

	pwm1 = 50;
	pwm2 = 0;
	pwm3 = 0;
	pwm4 = 0;
		
}		

void Turn_left (void)
{
	pwm1 = 0;
	pwm2 = 0;
	pwm3 = 50;
	pwm4 = 0;
}

void Move_forward_loop(void)
{
	int i=0;
	int j = 0;
	while(j<=3){
			pwm1 = 0;
			pwm2 = 50;
			pwm3 = 0;
			pwm4 = 50;
			for(i=0; i<20000; i++);
			j++;}

			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 0;	
		
}

void Move_backward_loop(void)
{
	int i=0;
	int j = 0;
	while(j<=3){
			pwm1 = 50;
			pwm2 = 0;
			pwm3 = 50;
			pwm4 = 0;
			for(i=0; i<20000; i++);
			j++;}

			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 0;	
		
}

void Turn_left_45(void)
{
	
	int i=0;
	int j = 0;
	while(j<=7){
			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 50;
			for(i=0; i<10000; i++);
			j++;}

			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 0;

}

void Turn_right_45 (void)
{
	int i=0;
	int j = 0;
	while(j<=7){
			pwm1 = 0;
			pwm2 = 50;
			pwm3 = 0;
			pwm4 = 0;
			for(i=0; i<10000; i++);
			j++;}

			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 0;
}	

void Turn_right_45_spin (void)
{
	volatile int i=0;
	volatile int j = 0;
	while(j<=7){
			pwm1 = 0;
			pwm2 = 50;
			pwm3 = 0;
			pwm4 = 0;
			for(i=0; i<3620; i++);
			j++;}

			pwm1 = 0;
			pwm2 = 0;
			pwm3 = 0;
			pwm4 = 0;
}

void Move_forward(void)
{

	pwm1 = 100;
	pwm2 = 0;
	pwm3 = 100;
	pwm4 = 0;	
	
}	

void Move_backward(void)
{

	pwm1 = 0;
	pwm2 = 100;
	pwm3 = 0;
	pwm4 = 100;
		
}

void stop(void) {
	pwm1 = 0;
	pwm2 = 0;
	pwm3 = 0;
	pwm4 = 0;
}

void parallel_park (void)
{
	Turn_left_45();
	Move_forward_loop();
	Turn_right_45();
	Move_backward_loop();
}

void level_function(void)
{
	int state_RL=0;
	int state_BF=0;
	
	state_RL = movement_logic_RL(volt_right, volt_left, (max(volt_right, volt_left)*TH_PERCENT_TURN) );
	switch(state_RL) {
		case RIGHT:
			Turn_right();
			break;
		case LEFT:
			Turn_left();
			break;
		case 0:
			if (state_BF == STOP)
				stop();
			break;
	}
	
	wait_one_and_half_bit_time();
	
	state_BF = movement_logic_BF(volt_right, volt_left, levels[level]*thresholds[level] , levels[level]);
	switch(state_BF) {	
		case FOR:
			Move_forward();
			break;
		case BACK:
			Move_backward();
			break;
		case STOP:
			if (state_RL == STOP) 
				stop();
			break;
	}
	
	wait_one_and_half_bit_time();
}

void spin_o_rama (void)
{
int m=0;
while( m <= 3)
	{
	Turn_right_45_spin();
	m++;
	}
}






