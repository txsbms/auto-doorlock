#include "fsl_device_registers.h"

void MotorLock() {
	for (unsigned j = 0; j < 13; ++j) {
		GPIOD_PDOR = 0x36;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x3A;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x39;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x35;
		for (unsigned long i = 0; i < 25000; ++i);
	}
}

void MotorUnlock() {
	for (unsigned j = 0; j < 13; ++j) {
		GPIOD_PDOR = 0x35;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x39;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x3A;
		for (unsigned long i = 0; i < 25000; ++i);
		GPIOD_PDOR = 0x36;
		for (unsigned long i = 0; i < 25000; ++i);
	}
}

unsigned int GetCurrVal() {
	if ((GPIOB_PDIR & 0x04) == 0x04) {
		return 1;
	}
	else {
		return 0;
	}
}
int main(void) {
	/*Enable Clock Gate Control*/
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

	/*Enable GPIO*/
	PORTB_GPCLR = 0x40100;
	PORTD_GPCLR = 0x3F0100;

	/*Configure Direction*/
	GPIOB_PDDR = 0x00;
	GPIOD_PDDR = 0x3F;

	unsigned int prevVal = 0;
	while(1) {
		if (GetCurrVal() != prevVal) {
			if (GetCurrVal() == 1) {
				prevVal = 1;
				MotorUnlock();
			}
			else {
				prevVal = 0;
				MotorLock();
			}
		}
	}
	return 0;
}
