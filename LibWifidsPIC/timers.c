/**
* @file timers.c
* @description Manage dsPIC33 timers
* @author Felipe Gil
* @date 2018-1-10
* @version 0.1 2018-1-10
*/

#include "timers.h"

// ---------------------------------------------------------------
// ---------------- PRIVATE FUNCTIONS PROTOTYPES -----------------
// ---------------------------------------------------------------

void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void);
void __attribute__((interrupt,no_auto_psv)) _T2Interrupt(void);
//void __attribute__((interrupt,no_auto_psv)) _T3Interrupt(void);


// ---------------------------------------------------------------
// ---------------------- PUBLIC FUNCTIONS -----------------------
// ---------------------------------------------------------------

void iniTimer1(uint32_t period_ms)
{
	periodT1 = period_ms;
	countT1 = 0;
	//count 1 ms with T1
	TMR1 = 0;
	T1CONbits.TCKPS = 1;
	PR1 = ((FCY/PREESC1)/1000);
	IFS0bits.T1IF = 0;	//clear interruption flag
	IEC0bits.T1IE = 1;	//enable interruptions
	T1CONbits.TON = 1;	//enable timer (start counting)
}

void iniTimer2(uint32_t period_ms)
{
	periodT2 = period_ms;
	countT2 = 0;
	//count 1 ms with T2
	TMR2 = 0;
	T2CONbits.TCKPS = 1;
	PR2 = ((FCY/PREESC1)/1000);
	IFS0bits.T2IF = 0;	//clear interruption flag
	IEC0bits.T2IE = 1;	//enable interruptions
	T2CONbits.TON = 1;	//enable timer (start counting)
}

int endTimer1(void)
{
	int end = 0;
	if(countT1 == periodT1){
		IEC0bits.T1IE = 0;	//disable interruptions
		T1CON = 0x0000;		//disable T1	
		countT1 = 0;		//reset T1 count
		end = 1;
	}
	return end;
}

int endTimer2(void)
{
	int end = 0;
	if(countT2 == periodT2){
		IEC0bits.T2IE = 0;	//disable interruptions
		T2CON = 0x0000;		//disable T1	
		countT2 = 0;		//reset T1 count
		end = 1;
	}
	return end;
}

void delay(uint32_t period_ms)
{
	uint32_t countT3 = 0;
	//count 1 ms with T3
	TMR3 = 0;
	T3CONbits.TCKPS = 1;
	PR3 = ((FCY/PREESC1)/1000); //1 ms
	IFS0bits.T3IF = 0;	//clear interruption flag
	IEC0bits.T3IE = 0;	//enable interruptions
	T3CONbits.TON = 1;	//enable timer (start counting)
	while(countT3 < period_ms){	//delay for the selected number of ms
		while(!IFS0bits.T3IF);	//wait for interruption flag
		IFS0bits.T3IF = 0;		//clear flag
		countT3++;
	}
	IEC0bits.T3IE = 0;	//disable interruptions
	T3CONbits.TON = 0;	//disable timer
	return;
}

void __attribute__((interrupt,no_auto_psv)) _T1Interrupt(void)
{
	IFS0bits.T1IF = 0;
	countT1++;
}

void __attribute__((interrupt,no_auto_psv)) _T2Interrupt(void){
	IFS0bits.T2IF = 0;
	countT2++;
}
