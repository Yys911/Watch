/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Author:	yiyison
*/


#include "watch.h"
#include "TimerOne.h"


// the setup function runs once when you press reset or power the board
void setup() {
    /*
      I/O pin setups
    */
    attachInterrupt(digitalPinToInterrupt(pin), RECEIVE, mode);  //set RECEIVE as 																	 interrupt function

    Timer1.initialize(500000);		//initialise Timer1, interrupt every 0.5s
    Timer1.attachInterrupt(SEND);		//set SEND as interrupt functions
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
