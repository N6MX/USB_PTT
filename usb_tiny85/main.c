/**
 * Project: AVR ATtiny USB Tutorial at http://codeandlife.com/
 * Author: Joonas Pihlajamaa, joonas.pihlajamaa@iki.fi
 * Inspired by V-USB example code by Christian Starkjohann
 * Copyright: (C) 2012 by Joonas Pihlajamaa
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv.h"

//Function declarations
USB_PUBLIC uchar usbFunctionSetup(uchar data[8]);
void hadUsbReset();

#define USB_LED_OFF 0x00
#define USB_LED_ON 0x01
#define LED_PIN (1<<PB3)

//Main program
int main() {
	uchar i;

	DDRB = LED_PIN;					//Setup LED pin as output
	
    wdt_enable(WDTO_1S);			//Enable 1s watchdog timer

    usbInit();						//Initialize V-USB library

	//Enforce re-enumeration
	//Watchdog automatically resets chip after 1 second if freeze occurs
    usbDeviceDisconnect();
    for(i = 0; i<250; i++) {		//Wait 500 ms
        wdt_reset();				//Keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();
	
    sei();							//Enable interrupts after re-enumeration
	
    while(1) {
        wdt_reset();				//Keep the watchdog happy
        usbPoll();
    }
	
    return 0;
}


/************************************************************
********************SUPPORTING FUNCTIONS*********************
************************************************************/

// this gets called when custom control message is received
USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (void *)data; // cast data to correct type
	
	switch(rq->bRequest) { // custom command is in the bRequest field
    case USB_LED_ON:
		PORTB |= LED_PIN; // turn LED on
		return 0;
	case USB_LED_OFF: 
		PORTB &= ~LED_PIN; // turn LED off
		return 0;
    }

    return 1; // should not get here
}

#define abs(x) ((x) > 0 ? (x) : (-x))

// Called by V-USB after device reset
void hadUsbReset() {
    int frameLength, targetLength = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
    int bestDeviation = 9999;
    uchar trialCal, bestCal, step, region = 0;
    bestCal = 0;

    // do a binary search in regions 0-127 and 128-255 to get optimum OSCCAL
    for(region = 0; region <= 1; region++) {
        frameLength = 0;
        trialCal = (region == 0) ? 0 : 128;
        
        for(step = 64; step > 0; step >>= 1) { 
            if(frameLength < targetLength) // true for initial iteration
                trialCal += step; // frequency too low
            else
                trialCal -= step; // frequency too high
                
            OSCCAL = trialCal;
            frameLength = usbMeasureFrameLength();
            
            if(abs(frameLength-targetLength) < bestDeviation) {
                bestCal = trialCal; // new optimum found
                bestDeviation = abs(frameLength -targetLength);
            }
        }
    }

    OSCCAL = bestCal;
}

