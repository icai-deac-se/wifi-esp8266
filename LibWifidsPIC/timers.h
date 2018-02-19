/**
* @file timers.h
* @description Manage dsPIC33 timers
* @author Felipe Gil
* @date 2018-1-12
* @version 0.1 2018-1-12
*/

#ifndef _TIMERS_H_
#define _TIMERS_H_

#include "DriverPicTrainer/config.h"	//it contains FCY (clock freq)
//timer prescaler divisors
#define PREESC1 8	//for TCKPS = 1
#define PREESC2 64	//for TCKPS = 2
#define PREESC3 256	//for TCKPS = 3

//store last period in ms selected by the user (global to this module)
static uint32_t periodT1 = 0;
static uint32_t periodT2 = 0;
//count in ms correspondent to each timer (global to this module)
static uint32_t countT1 = 0;
static uint32_t countT2 = 0;


// --------------------------------------------------------------
// ---------------- PUBLIC FUNCTIONS PROTOTYPES -----------------
// --------------------------------------------------------------

/**
	* Start timer T1 with a period in ms.
	* Used in polling mode (check IFS0bits.T1IF)
	*/
void iniTimer1(uint32_t period_ms);

/**
	* Start timer T2 with a period in ms.
	* Used in polling mode (check IFS0bits.T2IF)
	*/
void iniTimer2(uint32_t period_ms);

/**
	* If timer T1 has ended, return true 
	* and switch off the timer.
	*/
bool endTimer1(void);

/**
	* If timer T2 has ended, return true 
	* and switch off the timer.
	*/
bool endTimer2(void);

/**
	* Delay the selected period in ms
	* (using timer T3).
	*/
void delay(uint32_t period_ms);

#endif