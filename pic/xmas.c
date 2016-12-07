//
// PIC16F1705 code for the XmasTree
// Project located at github.com/SmallRoomLabs/xmastree
// Copright (c) 2016 Mats Engstrom SmallRoomLabs
// Released under the MIT license
//

#define _XTAL_FREQ 32000000

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>

#define T0_DIV256 0b0111
#define T0_DIV128 0b0110
#define T0_DIV64  0b0101
#define T0_DIV32  0b0100
#define T0_DIV16  0b0011
#define T0_DIV8   0b0010
#define T0_DIV4   0b0001
#define T0_DIV2   0b0000
#define T0_DIV1   0b1000

// PIC16F1705 Configuration Bit Settings
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCDDIS = ON      // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR)
#pragma config PLLEN = ON       // Phase Lock Loop enable (4x PLL is always enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config IDLOC0 = 0x01	// Set the User ID data to some
// #pragma config IDLOC1 = 0x23    // values for testing
// #pragma config IDLOC2 = 0x45
// #pragma config IDLOC3 = 0x56

// Defines of bitmasks to make the charlieplex table easier to write
#define b0 (1<<0)   
#define b1 (1<<1)
#define b2 (1<<2)
#define b3 (1<<3)
#define b4 (1<<4)
#define b5 (1<<5)


#define NUMBEROFLEDS  45    // The tree have 45 LEDs in total

//
// This table holds the values for the TRIS and LAT reigsters to
// setup the GPIOs correctly to light up the specified LED
//
const uint8_t charlie[NUMBEROFLEDS*4]={
// TRISA         TRISC         LATA LATC
// ============  ============  ==== ====
   0xFF^(b5|b4), 0xFF^0,       b5,  0,   //  0 1a-5a  RING1
   0xFF^(b5|b4), 0xFF^0,       b4,  0,   //  1 5a-1a
   0xFF^b5,      0xFF^b0,      b5,  0,   //  2 1a-6c
   0xFF^b5,      0xFF^b0,      0,   b0,  //  3 6c-1a
   0xFF^b5,      0xFF^b3,      b5,  0,   //  4 1a-7c
   0xFF^b5,      0xFF^b3,      0,   b3,  //  5 7c-1a
   0xFF^b5,      0xFF^b4,      b5,  0,   //  6 1a-8c
   0xFF^b5,      0xFF^b4,      0,   b4,  //  7 8c-1a

   0xFF^0,       0xFF^(b4|b5), 0,   b5,  //  8 C4 C5  RING2
   0xFF^0,       0xFF^(b4|b5), 0,   b4,  //  9 C5 C4
   0xFF^b5,      0xFF^b5,      b5,  0,   // 10 A5 C5
   0xFF^b5,      0xFF^b5,      0,   b5,  // 11 C5 A5
   0xFF^b5,      0xFF^b1,      b5,  0,   // 12 A5 C1
   0xFF^b5,      0xFF^b1,      0,   b1,  // 13 C1 A5
   0xFF^b5,      0xFF^b2,      b5,  0,   // 14 A5 C4
   0xFF^b5,      0xFF^b2,      0,   b2,  // 15 C4 A5

   0xFF^0,       0xFF^(b2|b5), 0,   b5,  // 16 RC5 RC2  RING3
   0xFF^0,       0xFF^(b2|b5), 0,   b2,  // 17 RC2 RC5
   0xFF^b4,      0xFF^b5,      0,   b5,  // 18 RC5 RA4
   0xFF^b4,      0xFF^b5,      b4,  0,   // 19 RA4 RC5
   0xFF^0,       0xFF^(b0|b5), 0,   b5,  // 20 RC5 RC0
   0xFF^0,       0xFF^(b0|b5), 0,   b0,  // 21 RC0 RC5
   0xFF^0,       0xFF^(b3|b5), 0,   b5,  // 22 RC5 RC3
   0xFF^0,       0xFF^(b3|b5), 0,   b3,  // 23 RC3 RC5

   0xFF^0,       0xFF^(b0|b1), 0,   b1,  // 24 C1 C0  RING4
   0xFF^0,       0xFF^(b0|b1), 0,   b0,  // 25 C0 C1
   0xFF^0,       0xFF^(b1|b3), 0,   b1,  // 26 C1 C3
   0xFF^0,       0xFF^(b1|b3), 0,   b3,  // 27 C3 C1
   0xFF^0,       0xFF^(b1|b4), 0,   b1,  // 28 C1 C4
   0xFF^0,       0xFF^(b1|b4), 0,   b4,  // 29 C4 C1
   0xFF^0,       0xFF^(b1|b5), 0,   b5,  // 30 C5 C1
   0xFF^0,       0xFF^(b1|b5), 0,   b1,  // 31 C1 C5

   0xFF^0,       0xFF^(b2|b3), 0,   b2,  // 32 C2 C3  RING 5
   0xFF^0,       0xFF^(b2|b3), 0,   b3,  // 33 C3 C2
   0xFF^0,       0xFF^(b2|b4), 0,   b2,  // 34 C2 C4
   0xFF^0,       0xFF^(b2|b4), 0,   b4,  // 35 C4 C2
   0xFF^0,       0xFF^(b1|b2), 0,   b1,  // 36 C1 C2
   0xFF^0,       0xFF^(b1|b2), 0,   b2,  // 37 C2 C1
   0xFF^b4,      0xFF^b1,      0,   b1,  // 38 A4 C1
   0xFF^b4,      0xFF^b1,      b4,  0,   // 39 C1 A4

   0xFF^b4,      0xFF^b3,      b4,  0,   // 40 A4 C3  RING 6 (SMALL)
   0xFF^b4,      0xFF^b3,      0,   b3,  // 41 C3 A4
   0xFF^b4,      0xFF^b4,      b4,  0,   // 42 A4 C4
   0xFF^b4,      0xFF^b4,      0,   b4,  // 43 C4 A4

   0xFF^b4,      0xFF^b0,      0,   b0,  // 44 C0 A4  RING 7 (SINGLE)
};

volatile uint8_t tick=0;                // Incremened by IRQ at 0.1s rate
volatile uint8_t ledsOn[NUMBEROFLEDS];
volatile uint8_t maxLitLeds=1;
volatile uint8_t done=0;

//
// 0          1          2          3           4          5          6
// 01234567 89012345 67890123 45678901 23456789 01234567 89012345 67890123
//
// 01234567 01234567 01234567 01234567 01234567 01234567 01234567 01234567
//
// LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLLLL LLLLLmmm mmmmdddd dddddddd
//


//
// Delays the specified time. Return true if button is pressed
//
uint8_t ButtonDelay(uint8_t ms) {
  while (ms-- > 0) {
    __delay_ms(1);
  }
  if (done) return 1;
  return 0;
}


// Macro for turning off all LEDs
#define AllLedOff()  memset(ledsOn,0xFF,NUMBEROFLEDS)

//
// Turn of all LEDs in the specified ring
//
void RingOn(uint8_t ring, uint8_t clear) {
  if (clear) AllLedOff();
  if (ring<5) {
      for (uint8_t i=0; i<8; i++) {
        ledsOn[i]=i+ring*8;
      }
  } else if (ring==5) {
    ledsOn[0]=40;
    ledsOn[1]=41;
    ledsOn[2]=42;
    ledsOn[3]=43;
  } else {
    ledsOn[0]=44;
  }
}

//
// Show animated rings.
//   mode=0 - Go up from largest to smallest
//   mode=1 - Go down from smallest to largest
//   mode-2 - Go up and down
//
void RingUpDown(uint8_t mode) {
  maxLitLeds=8;
  do {
    if (mode==0 || mode==2) {
      for (uint8_t i=0; i<7; i++) {
        RingOn(i,1);  
        if (ButtonDelay(200)) {done=1; break;}
      }
    } 

    if (mode==1) {
      for (uint8_t i=6; i<=6; i--) {
        RingOn(i,1);  
        if (ButtonDelay(200)) {done=1; break;}
      }
    }
    if (mode==2) {
      for (uint8_t i=5; i>0; i--) {
        RingOn(i,1);
        if (ButtonDelay(200)) {done=1; break;}
      }
    }
  } while (!done);
}


//
// Make the top two rings fade in/out while
// flashing a random pixel at the remaining rinds
//
void PulseTopRandomPixel() {
  int8_t dir=1;

  AllLedOff();
  maxLitLeds=1;
  ledsOn[0]=44;
  ledsOn[1]=43;
  ledsOn[2]=42;
  ledsOn[3]=41;
  ledsOn[4]=40;
  do {
    for (uint16_t i=0; i<7; i++) {
      if ((rand()&127)>120) { 
        ledsOn[5]=rand()%40;
      } else {
        ledsOn[5]=0xFF;
      }
    if (ButtonDelay(5)) done=1;
    }
    maxLitLeds+=dir;
    if (maxLitLeds==30) dir=-1;
    if (maxLitLeds==5) dir=1;
  } while (!done);
}



//
// Raindrops falling down from top LED
//

void Rain() {
  uint8_t cnt=0;

  const uint8_t below40[]={32,33,33,34}; 
  const uint8_t below41[]={34,35,35,36};
  const uint8_t below42[]={36,37,37,38};
  const uint8_t below43[]={38,39,39,32};

  maxLitLeds=4;
  AllLedOff();
  do {
    if (cnt++ == 1) {
      cnt=0;
      if (ledsOn[0]==0xFF) {
        ledsOn[0]=44;
      } else if (ledsOn[1]==0xFF) {
        ledsOn[1]=44;
      } else if (ledsOn[2]==0xFF) {
        ledsOn[2]=44;
      } else if (ledsOn[3]==0xFF) {
        ledsOn[3]=44;
      }
    }
    if (ButtonDelay(150)) done=1;
    for (uint8_t i=0; i<maxLitLeds; i++) {
      if (ledsOn[i]==0xFF) continue;
      if (ledsOn[i]==44) {
        ledsOn[i]=40+(rand()&0x03);
      } else if (ledsOn[i]==40) { 
        ledsOn[i]=below40[rand()&0x03];
      } else if (ledsOn[i]==41) { 
        ledsOn[i]=below41[rand()&0x03];
      } else if (ledsOn[i]==42) { 
        ledsOn[i]=below42[rand()&0x03];
      } else if (ledsOn[i]==43) { 
        ledsOn[i]=below43[rand()&0x03];
      } else if (ledsOn[i]>=8) {
        ledsOn[i]-=8;
      } else {
        ledsOn[i]=0xff;
      }
    }
  } while (!done);
}


//
// Random blibs that gets more and more then less
//
void RandomSlowBlips() {
  do {
    for (maxLitLeds=1; maxLitLeds<10; maxLitLeds++) {
      for (uint8_t j=0; j<2; j++) { 
        AllLedOff();
        for (uint8_t i=0; i<maxLitLeds; i++) {
          ledsOn[i]=rand()%NUMBEROFLEDS;
        }    
        if (ButtonDelay(200)) return;
      }
    }
    for (maxLitLeds=9; maxLitLeds>1; maxLitLeds--) {
      for (uint8_t j=0; j<2; j++) { 
        AllLedOff();
        for (uint8_t i=0; i<maxLitLeds; i++) {
          ledsOn[i]=rand()%NUMBEROFLEDS;
        }    
        if (ButtonDelay(200)) return;
      }
    }
  } while (!done);
}


//
// Quck flickering of all leds
//
void Frussel() {
  maxLitLeds=8;
  do {
    for (uint8_t i=0; i<maxLitLeds; i++) {
      ledsOn[i]=rand()%NUMBEROFLEDS;
    }    
    if (ButtonDelay(1)) return;
  } while (!done);
}


//
//
//
void SpiralUpDown() {
  for (uint8_t i=0; i<NUMBEROFLEDS; i++) ledsOn[i]=i;
  do {
    for (maxLitLeds=1; maxLitLeds<NUMBEROFLEDS; maxLitLeds++) {
      if (ButtonDelay(50)) return;
    }      
    for (maxLitLeds=NUMBEROFLEDS; maxLitLeds>1; maxLitLeds--) {
      if (ButtonDelay(50)) return;
    }      
  } while (!done); 
}


//
//
//
void SpiralAway() {
  AllLedOff();
  maxLitLeds=NUMBEROFLEDS;
  do {
    for (uint8_t i=0; i<NUMBEROFLEDS; i++) {
      ledsOn[i]=i;
      if (ButtonDelay(50)) return;
    }
    if (ButtonDelay(50)) return;
    for (uint8_t i=0; i<NUMBEROFLEDS; i++) {
      ledsOn[i]=0xFF;
      if (ButtonDelay(50)) return;
    }
  } while (!done); 
}


//
//
//
void main() {
  OSCCON=0b11110000;		// Use internal oscillator set at 32MHz

  INTCON=0;             // Disable all interrupts
  PIE1=0;               // Disable peripheral interrupts
  PIE2=0;               // Disable more peripheral interrupts
  PIE3=0;               // Disable even more peripheral interrupts
  IOCAN=0x00;           // Disable pin-change interrupts
  IOCAP=0x00;
  IOCCN=0x00;
  IOCCP=0x00;

  ANSELA=0;			        // All GPIOs are digital
  ANSELC=0;
  WPUA=(1<<2);			    // Turn on pullup for the button
  WPUC=0;			          // No pullups at port C
  TRISA=0xFF;			      // All GPIOs are input until the charlieplexing in the IRQ
  TRISC=0xFF;           //   says otherwise

  //
  // Setup Timer0 to generate interrupts at about 4KHz for refreshing
  // the LEDs.  Even if all 45 LEDs are lit/processed the refresh rate
  // will be a flicker-free 87Hz.
  //
  //   fOsc/4      Prescale     8bit counter   45 Leds      
  // 32000000/4 -> 8000000/8 -> 1000000/256 -> 3926/45 -> 87 Hz Refresh 
  //
  OPTION_REG=0x00 | T0_DIV8;    // Wpu enabled, Prescale T0 from Fosc/4 with 8 
  INTCONbits.T0IE=1;            // Enable interrupt on TMR0 overflow
  INTCONbits.GIE=1;             // Global interrupt enable

  AllLedOff();
  for (;;) {
    PulseTopRandomPixel(); while (done);
    RingUpDown(0); while (done);
    Rain(); while (done);
    SpiralAway(); while(done);
    RingUpDown(1); while (done);
    RandomSlowBlips(); while(done);
    Frussel(); while(done);
    RingUpDown(2); while (done);
    SpiralUpDown(); while(done);
  }
}


//
// Interrupt handler. Takes care of the refreshing of the charlieplexed LEDs by interating
// the list of leds to be lit ledsOn[].  The ledsOn[] array holds the number/id of the LEDs
// to be lit, unused entries in the array are set to 0xFF.
//
void interrupt ISR(void) {
  static uint8_t cnt=0;         // Index into the ledsOn[] array
  static uint16_t localTick=0;  // Internal counter keepint track of the 0.1s tick

  if(INTCONbits.T0IF) {         // If timer flag is set
    INTCONbits.T0IF = 0;        // Clear the interrupt flag 

    if (localTick++>393) {      // Locally count up to 393 before incrementing
      localTick=0;              //     the external 0.1s tick counter
      tick++;

      if (tick>250 && tick<254) done=1;
      if (tick>=254) {
        done=0;
        tick=0;
      }
    }

    cnt++;
    if (cnt>=maxLitLeds) cnt=0; // Only care about the first "maxLitLeds" in the ledsOn[] array 
    if (ledsOn[cnt]==0xFF) {    // If current entry is no-led then turn of GPIOs
      TRISA=0xFF;
      TRISC=0xFF;
      LATA=0x00;
      LATC=0x00;
    } else {
      uint8_t t=ledsOn[cnt]*4;  // The specified LED should be on. Index into the charlineplexing
      TRISA=charlie[t];         // table and use the values there to setup the GPIOs
      TRISC=charlie[t+1];
      LATA=charlie[t+2];
      LATC=charlie[t+3];
    }
  }

}  
