
#include "pins.h"


void config_pins_as_output() {
	SIM->SCGC5 |= (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13); //Enable to B,C,D,E
	
	//Enable Pins as GPIO
	PORTE->PCR[24] = (1 <<  8); //R1 is E24
	PORTD->PCR[1] = (1 <<  8); //G1 is D1
	PORTE->PCR[25] = (1 <<  8); //B1 is E25
	
	PORTD->PCR[3] = (1 <<  8); //R2 is D3
	PORTD->PCR[0] = (1 <<  8); //G2 is D0
	PORTD->PCR[2] = (1 <<  8); //B2 is D2
	
	PORTB->PCR[2] = (1 <<  8); //A is B2
	PORTB->PCR[3] = (1 <<  8); //B is B3
	PORTB->PCR[10] = (1 <<  8);//C is B10
	PORTB->PCR[11] = (1 <<  8); //LAT is B11
	PORTC->PCR[4] = (1 <<  8); //CLK is C4
	PORTC->PCR[12] = (1 <<  8); //OE is C12 
	
	//Make them output pins
	PTB->PDDR = (1 << 2 | 1 << 3 | 1 << 10 | 1 << 11 );
	PTC->PDDR = (1 << 4 | 1 << 12 );  
	PTD->PDDR = (1 << 1 | 1 << 2 | 1 << 3 | 1 << 0 ); 
	PTE->PDDR = (1 << 24 | 1 << 25 );  	
}

void digital_write(int pin , int high) {
	if(pin == 0) {
		if(high) {
			PTE->PSOR |= (1 << 24);
		}
		else {
			PTE->PCOR |= (1 << 24);
		}
	}
	else if(pin == 1) {
		if(high) {
			PTD->PSOR |= (1 << 1);
		}
		else {
			PTD->PCOR |= (1 << 1);
		}
	}
	else if(pin == 2) {
		if(high) {
			PTE->PSOR |= (1 << 25);
		}
		else {
			PTE->PCOR |= (1 << 25);
		}
	}
	else if(pin == R2) {
		if(high) {
			PTD->PSOR |= (1 << 3);
		}
		else {
			PTD->PCOR |= (1 << 3);
		}
	}
	else if(pin == G2) {
		if(high) {
			PTD->PSOR |= (1 << 0);
		}
		else {
			PTD->PCOR |= (1 << 0);
		}
	}
	else if(pin == B2) {
		if(high) {
			PTD->PSOR |= (1 << 2);
		}
		else {
			PTD->PCOR |= (1 << 2);
		}
	}
	else if(pin == 6) {
		if(high) {
			PTB->PSOR |= (1 << 2);
		}
		else {
			PTB->PCOR |= (1 << 2);
		}
	}
	else if(pin == 7) {
		if(high) {
			PTB->PSOR |= (1 << 3);
		}
		else {
			PTB->PCOR |= (1 << 3);
		}
	}
	else if(pin == 8) {
		if(high) {
			PTB->PSOR |= (1 << 10);
		}
		else {
			PTB->PCOR |= (1 << 10);
		}
	}
	else if(pin == 9) {
		if(high) {
			PTB->PSOR |= (1 << 11);
		}
		else {
			PTB->PCOR |= (1 << 11);
		}
	}
	else if(pin == 10) {
		if(high) {
			PTC->PSOR |= (1 << 4);
		}
		else {
			PTC->PCOR |= (1 << 4);
		}
	}
	else if(pin == 11) {
		if(high) {
			PTC->PSOR |= (1 << 12);
		}
		else {
			PTC->PCOR |= (1 << 12);
		}
	}
}

