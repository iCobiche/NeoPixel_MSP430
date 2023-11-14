#include <msp430.h> 
#include <std_types.h>

/////////////////////  DEVICE PINOUTS  ///////////////////
//
//                       MSP4302332
//                      ____________
//                     |            |
//              P1.0 1 |*           | 16 DVCC
//              P1.1 2 |            | 15 AVCC
//              P1.2 3 |            | 14 DVSS
//              P1.3 4 |            | 13 AVSS
//              P1.4 5 |            | 12 P2.6
//              P1.5 6 |            | 11 P2.7
//              P1.6 7 |            | 10 TEST
//              P1.7 8 |            |  9 RST
//                     |____________|
//             
// 
/////////////////////  PIN DISTRIBUTIONS  ///////////////////
//
// PIN  | FUNCTION                        | CONNECTION
//------+---------------------------------+------------------
//  01  | P1.0/TA0CLK/ACLK/A0             | PUSH BUTTON
//  02  | P1.1/TA0.0/A1                   | -
//  03  | P1.2/TA0.1/A2                   | DIN (WS2812B D1)
//  04  | P1.3/ADC10CLK/A3/VREF-/VEREF    | -
//  05  | P1.4/SMCLK/A4/VREF+/VEREF+/TCK  | -
//  06  | P1.5/TA0.0/SCLK/A5/TMS          | -
//  07  | P1.6/TA0.1/SDO/SCL/A6/TDI/TCLK  | -
//  08  | P1.7/SDI/TDO/TDI                | -
//  09  | RST/NMI/SBWTDIO                 | JTAG
//  10  | TEST/SBWTCK                     | JTAG
//  11  | XOUT/P2.7                       | -
//  12  | XIN/P2.6/TA0.1                  | -
//  13  | AVSS                            | GND
//  14  | DVSS                            | GND
//  15  | AVCC                            | 3.3V
//  16  | DVCC                            | 3.3V
//------+---------------------------------+------------------

//***** CONFIGURATION *******
#define DIN_PIN         (BIT2)            // Set output pin
#define BUTTON_PIN      (BIT0)            // Set input pin
#define NUM_LEDS        (10u)             // Number of leds
#define DIRECT_LOGIC    (1u)              // 1: Direct logic; 0: Inverted logic

//***** MACROS *******
#define delay(x)        if(change == 1) { return; } __delay_cycles(x);

//***** FUNCTIONS *******
void NEO_boardInit(void);
void NEO_portInit(void);
void NEO_applRun(void);
void NEO_sendByte(uint8 b);
void NEO_sendLEDs();
void NEO_setLEDColor(uint16 led, uint8 red, uint8 green, uint8 blue);
void NEO_clearStrip(void);
// Animations
void breatheAnimation(uint8 red, uint8 green, uint8 blue);
void raceAnimation(uint8 red, uint8 green, uint8 blue);
void rainbowAnimation();
void blinkAnimation(uint8 red, uint8 green, uint8 blue);
void waveAnimation(uint8 red, uint8 green, uint8 blue);
void centerToOutAnimation(uint8 red, uint8 green, uint8 blue);
void pingPongAnimation(uint8 red, uint8 green, uint8 blue);
void fadeInOutAnimation(uint8 red, uint8 green, uint8 blue);
void buildUpAnimation(uint8 red, uint8 green, uint8 blue);


//***** GLOBAL VARIABLES *******
uint8 state  = 0u;
uint8 change = 0u;
uint8 leds[NUM_LEDS * 3u];


//***** MAIN FUNCTION *******
void main(void)
{
  // Initial configuration
  NEO_boardInit();

  // Port configuration
  NEO_portInit();

  // Enable interrupt
  __enable_interrupt();

NEXT_COLOR:
  state++;
  // Configurar los colores de los LEDs
  NEO_clearStrip();

  // Main loop
  while(1)
  {
    NEO_applRun();
    if(change == 1u)
    {
      change = 0u;
      goto NEXT_COLOR;
    }
  }
}

// Initial configuration
void NEO_boardInit(void)
{
  // Stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;

  // Configure clock to 16 MHz
  DCOCTL    = CALDCO_16MHZ;
  BCSCTL1   = CALBC1_16MHZ;
}

// Port configuration
void NEO_portInit(void)
{
  // PORT 1
  P1IN    = 0u;
  #if DIRECT_LOGIC == 1
  P1OUT   = BUTTON_PIN;
  #else
  P1OUT   = DIN_PIN;
  #endif
  P1DIR   = DIN_PIN;
  P1IFG   = 0u;
  P1IES   = BUTTON_PIN;
  P1IE    = BUTTON_PIN;
  P1SEL   = 0u;
  P1SEL2  = 0u;
  P1REN   = BUTTON_PIN;

  // PORT 2
  P2IN    = 0u;
  P2OUT   = 0u;
  P2DIR   = 0u;
  P2IFG   = 0u;
  P2IES   = 0u;
  P2IE    = 0u;
  P2SEL   = 0u;
  P2SEL2  = 0u;
  P2REN   = 0u;
}

// Main runnable
__inline void NEO_applRun(void)
{
  switch(state)
  {
    case 0:
      NEO_clearStrip();
      break;
    case 1:
      breatheAnimation(0xFF, 0x00, 0xFF);
      break;
    case 2:
      raceAnimation(0x00, 0xFF, 0xFF);
      break;
    case 3:
      rainbowAnimation();
      break;
    case 4:
      blinkAnimation(0xFF, 0xFF, 0x00);
      break;
    case 5:
      waveAnimation(0x00, 0x00, 0xFF);
      break;
    case 6:
      centerToOutAnimation(0x00, 0xFF, 0x00);
      break;
    case 7:
      pingPongAnimation(0xFF, 0x00, 0x00);
      break;
    case 8:
      fadeInOutAnimation(0x80, 0x00, 0x80);
      break;
    case 9:
      buildUpAnimation(0xFF, 0x9C, 0x00);
      break;
    default:
      state = 0; // Reiniciar la máquina de estados si el estado es desconocido
      break;
  }
}

// Function to send one byte to NeoPixel. Will be called as many times as there are LEDs.
void NEO_sendByte(uint8 b)
{
  uint16 i;

  for (i = 0u; i < 8u; i++)
  {
    #if DIRECT_LOGIC == 1
    if (b & 0x80)
    {
      P1OUT |= DIN_PIN;
      __delay_cycles(11);
      P1OUT &= ~DIN_PIN;
    }
    else
    {
      P1OUT |= DIN_PIN;
      __delay_cycles(1);
      P1OUT &= ~DIN_PIN;
    }
    #else
    if (b & 0x80)
    {
      P1OUT &= ~DIN_PIN;
      __delay_cycles(11);
      P1OUT |= DIN_PIN;
    }
    else
    {
      P1OUT &= ~DIN_PIN;
      __delay_cycles(1);
      P1OUT |= DIN_PIN;
    }
    #endif
    b <<= 1;
  }
}

// Function to send each color to each Neopixel
void NEO_sendLEDs()
{
  uint16 i;
  for (i = 0u; i < NUM_LEDS * 3u; i++)
  {
    NEO_sendByte(leds[i]);
  }

  // Mandatory delay
  delay(800);
}

// Function to set the colors in each Neopixel
void NEO_setLEDColor(uint16 led, uint8 red, uint8 green, uint8 blue)
{
  if(led < NUM_LEDS)
  {
    uint16 idx = led * 3;
    leds[idx] = green;
    leds[idx + 1] = red;
    leds[idx + 2] = blue;
  }
}

// Function to turn off all NeoPixel
void NEO_clearStrip(void)
{
  uint16 i;
  for (i = 0u; i < NUM_LEDS; i++) {
    NEO_setLEDColor(i, 0u, 0u, 0u);
  }
  NEO_sendLEDs();
}

//***** ANIMATIONS *******
// Animation 1
void breatheAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i,j;
  
  for (i = 0; i < 255; i++)
  {
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    delay(160000);
  }

  for (i = 255; i > 0u; i--)
  {
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    delay(160000);
  }
  NEO_clearStrip();
}

// Animation 2
void raceAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_setLEDColor(i, red, green, blue);
    if (i > 0) NEO_setLEDColor(i - 1, 0, 0, 0);
    NEO_sendLEDs();
    delay(800000);
  }
  NEO_clearStrip();
}

// Animation 3
void rainbowAnimation()
{
  uint16 i,j;
  uint16 colors[7][3] =
  {
    {255, 0, 0},    // Red
    {255, 127, 0},  // Orange
    {255, 255, 0},  // Yellow
    {0, 255, 0},    // Green
    {0, 0, 255},    // Blue
    {75, 0, 130},   // Indigo
    {148, 0, 211}   // Violet
  };

  for (i = 0; i < 7; i++)
  { // 7 colores del arcoÃ­ris
    for (j = 0; j < NUM_LEDS; j++)
    {
      uint16 colorIndex = (i + j) % 7;
      NEO_setLEDColor(j, colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
    }
    NEO_sendLEDs();
    delay(1200000);
  }
}

// Animation 4
void blinkAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_setLEDColor(i, red, green, blue);
  }
  NEO_sendLEDs();
  delay(1600000);
  NEO_clearStrip();
  delay(1600000);
}

// Animation 5
void waveAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i,j;
  for (i = 0; i < NUM_LEDS * 2; i++)
  {
    for (j = 0; j < NUM_LEDS; j++)
    {
      uint16 delta = (i - j) * 255 / NUM_LEDS;
      if (delta < 0u) delta = -delta;
      delta = 255 - delta;
      NEO_setLEDColor(j, (red * delta) / 255, (green * delta) / 255, (blue * delta) / 255);
    }
    NEO_sendLEDs();
    delay(960000);
  }
}

// Animation 6
void centerToOutAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  uint16 center = NUM_LEDS / 2;
  for (i = 0; i <= center; i++)
  {
    NEO_setLEDColor(center - i, red, green, blue);
    NEO_setLEDColor(center + i, red, green, blue);
    NEO_sendLEDs();
    delay(800000);
  }
  NEO_clearStrip();
}

// Animation 6
void pingPongAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_clearStrip();
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    delay(800000);
  }
  for (i = NUM_LEDS - 2; i > 0; i--)
  {
    NEO_clearStrip();
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    delay(800000);
  }
}

// Animation 7
void fadeInOutAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i, j;
  for (i = 0; i < 255; i++)
  {
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    delay(80000);
  }
  for (i = 255; i > 0; i--)
  {
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    delay(80000);
  }
  NEO_clearStrip();
}

// Animation 8
void buildUpAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    delay(1200000);
  }
  NEO_clearStrip();
}


// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
  if(P1IFG & BUTTON_PIN)
  {
    change = 1u;
    P1IFG &= ~BUTTON_PIN;
  }
}
