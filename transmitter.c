void wait_bit_time(void) {
	_asm
		    mov R1, #2
		L1: mov R0, #184
		L0: djnz R0, L0
			djnz R1, L1
			ret
	_endasm;
}

void wait_delay(void) {
	_asm
		    mov R1, #255
		L2: mov R0, #255
		L3: djnz R0, L0
			djnz R1, L1
			ret
	_endasm;
}

void tx_byte ( unsigned char val )
{
	unsigned char j;
	//Send the start bit
	txon=1;
	wait_bit_time();
	for (j=0; j<8; j++)
	{
		txon=val&(0x01<<j)?1:0;
		wait_bit_time();
	}
	txon=1;
	//Send the stop bits
	wait_bit_time();
	wait_bit_time();
	txon=0;
}