#include <msp430.h> 
#include <std_types.h>
//#include "ws2812.h"

//***** MACROS *******
#define CHANGING()      if(change == 1u) { break; }
#define BTN_PRESSED()   if(((P1IN & BUTTON) == 0u) && (change == 0u)) change = 1u;
#define BTN_UNPRESSED() if(((P1IN & BUTTON) == 1u) && (change == 2u)) change = 0u;
#define BY_CHANGE()     if(change == 1u) { state++; change = 2; }

//***** DEFINITIONS *******
// Configuration
#define OUTPUT_PIN      (BIT2)      // Set output pin
#define BUTTON          (BIT0)      // Set input pin
#define NUM_LEDS        (10u)       // Number of leds
#define DIRECT_LOGIC    (1u)        // 1: Direct logic; 0: Inverted logic

//***** FUNCTIONS *******
// Logic
void NEO_applRun(void);
void NEO_boardInit(void);
void NEO_portInit(void);
void NEO_sendByte(uint8 b);
void NEO_sendLEDs(void);
void NEO_setLEDColor(uint16 led, uint8 red, uint8 green, uint8 blue);
void NEO_clearStrip(void);
// Animations
void breatheAnimation(uint8 red, uint8 green, uint8 blue);
void raceAnimation(uint8 red, uint8 green, uint8 blue);
void rainbowAnimation(void);
void blinkAnimation(uint8 red, uint8 green, uint8 blue);
void waveAnimation(uint8 red, uint8 green, uint8 blue);
void centerToOutAnimation(uint8 red, uint8 green, uint8 blue);
void pingPongAnimation(uint8 red, uint8 green, uint8 blue);
void fadeInOutAnimation(uint8 red, uint8 green, uint8 blue);
void buildUpAnimation(uint8 red, uint8 green, uint8 blue);

//***** GLOBAL VARIABLES *******
volatile uint16 state = 0u;
volatile uint16 change = 0u;
uint8 leds[NUM_LEDS * 3u];                // Array para almacenar los colores de cada LED (GRB - cada color es 1 byte)

//***** MAIN FUNCTION *******
void main(void)
{
  // Initial configuration
  NEO_boardInit();

  // Port configuration
  NEO_portInit();

  // Configurar los colores de los LEDs
  NEO_clearStrip();

  // Main loop
  while(1u)
  {
    NEO_applRun();
  }
}

// Main runnable
void NEO_applRun(void)
{
  switch(state)
  {
    case 0:
      NEO_clearStrip();
      break;
    case 1:
      breatheAnimation(0xFF, 0x00, 0xFF);       // Respiracion en rojo
      break;
    case 2:
      raceAnimation(0x00, 0xFF, 0xFF);          // Carrera en verde
      break;
    case 3:
      rainbowAnimation();                       // Arcoiris
      break;
    case 4:
      blinkAnimation(0xFF, 0xFF, 0x00);         // Parpadeo en azul
      break;
    case 5:
      waveAnimation(0x00, 0x00, 0xFF);          // Onda en amarillo
      break;
    case 6:
      centerToOutAnimation(0x00, 0xFF, 0x00);   //
      break;
    case 7:
      pingPongAnimation(0xFF, 0x00, 0x00);      //
      break;
    case 8:
      fadeInOutAnimation(0x80, 0x00, 0x80);     //
      break;
    case 9:
      buildUpAnimation(0xFF, 0x9C, 0x00);       //
      break;
    default:
      state = 0; // Reiniciar la máquina de estados si el estado es desconocido
      break;
  }
  BY_CHANGE();
  BTN_UNPRESSED();
}

void NEO_sendByte(uint8 b)
{
  uint16 i;

  for (i = 0u; i < 8u; i++)
  {
    #if DIRECT_LOGIC == 1
    if (b & 0x80)                 // Envia un 1
    {
      P1OUT |= OUTPUT_PIN;        // Encender el pin de datos
      __delay_cycles(11);         // Esperar 800ns
      P1OUT &= ~OUTPUT_PIN;       // Apagar el pin de datos
    }
    else                          // Envia un 0
    {
      P1OUT |= OUTPUT_PIN;        // Encender el pin de datos
      __delay_cycles(1);          // Esperar 300ns
      P1OUT &= ~OUTPUT_PIN;       // Apagar el pin de datos
    }
    #else
    if (b & 0x80)                 // Envia un 1
    {
      P1OUT &= ~OUTPUT_PIN;       // Encender el pin de datos
      __delay_cycles(11);         // Esperar 800ns
      P1OUT |= OUTPUT_PIN;        // Apagar el pin de datos
    }
    else                          // Envia un 0
    {
      P1OUT &= ~OUTPUT_PIN;       // Encender el pin de datos
      __delay_cycles(1);          // Esperar 300ns
      P1OUT |= OUTPUT_PIN;        // Apagar el pin de datos
    }
    #endif
    b <<= 1;                      // Mover al siguiente bit
  }
}

void NEO_sendLEDs()
{
  uint16 i;
  _disable_interrupt();           // Deshabilitar interrupciones globales
  for (i = 0u; i < NUM_LEDS * 3u; i++)
  {
    NEO_sendByte(leds[i]);
  }
  _enable_interrupt();            // Habilitar interrupciones globales

  // Mandatory delay
  __delay_cycles(800);           // Esperar 50 us

  BTN_PRESSED();
}

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
  P1IN    = 0u;
  #if DIRECT_LOGIC == 1
  P1OUT   = BUTTON;
  #else
  P1OUT   = OUTPUT_PIN;
  #endif
  P1DIR   = OUTPUT_PIN;
  P1IFG   = 0u;
  P1IES   = 0u;
  P1IE    = 0u;
  P1SEL   = 0u;
  P1SEL2  = 0u;
  P1REN   = BUTTON;

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

void NEO_clearStrip(void)
{
  uint16 i;
  for (i = 0u; i < NUM_LEDS; i++) {
    NEO_setLEDColor(i, 0u, 0u, 0u);
  }
  NEO_sendLEDs();
}

/*ANIMACIONES*/
// 1. AnimaciÃ³n de "Respiracion"
void breatheAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i,j;
  for (i = 0; i < 255; i++) { // Iluminar gradualmente
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(160000);         // Esperar delayMs milisegundos
  }
  for (i = 255; i > 0u; i--)       // Atenuar gradualmente
  {
    CHANGING();
    BTN_UNPRESSED();
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(160000);       // Esperar delayMs milisegundos
  }
  NEO_clearStrip();
  BY_CHANGE();
}

// 2. Animacion de "Carrera"
void raceAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++) {
    NEO_setLEDColor(i, red, green, blue);
    if (i > 0) NEO_setLEDColor(i - 1, 0, 0, 0); // Apagar el LED anterior
    NEO_sendLEDs();
    CHANGING();
    __delay_cycles(800000);
  }
  NEO_clearStrip(); // Limpiar la tira al final de la animaciÃ³n
}

// 3. AnimaciÃ³n de "Arcoiris"
void rainbowAnimation()
{
  uint16 i,j;
  uint16 colors[7][3] =
  {
    {255, 0, 0}, // Rojo
    {255, 127, 0}, // Naranja
    {255, 255, 0}, // Amarillo
    {0, 255, 0}, // Verde
    {0, 0, 255}, // Azul
    {75, 0, 130}, // Indigo
    {148, 0, 211} // Violeta
  };

  for (i = 0; i < 7; i++)
  { // 7 colores del arcoÃ­ris
    for (j = 0; j < NUM_LEDS; j++)
    {
      uint16 colorIndex = (i + j) % 7;
      NEO_setLEDColor(j, colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]);
    }
    NEO_sendLEDs();
    CHANGING();
    __delay_cycles(1200000);
  }
}

// 4. Animacion de "Parpadeo"
void blinkAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++) {
    NEO_setLEDColor(i, red, green, blue);
  }
  NEO_sendLEDs();
  __delay_cycles(1600000);
  NEO_clearStrip();
  __delay_cycles(1600000);
}

// 5. AnimaciÃ³n de "Onda"
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
    CHANGING();
    __delay_cycles(960000);
  }
}

// 1. Animación "Centro hacia Extremos"
void centerToOutAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  uint16 center = NUM_LEDS / 2;
  for (i = 0; i <= center; i++)
  {
    NEO_setLEDColor(center - i, red, green, blue);
    NEO_setLEDColor(center + i, red, green, blue);
    NEO_sendLEDs();
    CHANGING();
    __delay_cycles(800000);
  }
  NEO_clearStrip();
}

// 2. Animación "Ping-Pong"
void pingPongAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_clearStrip();
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(800000);
  }
  for (i = NUM_LEDS - 2; i > 0; i--)
  {
    CHANGING();
    BTN_UNPRESSED();
    NEO_clearStrip();
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(800000);
  }
  BY_CHANGE();
}

// 3. Animación "Fade In-Out"
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
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(80000);
  }
  for (i = 255; i > 0; i--)
  {
    CHANGING();
    BTN_UNPRESSED();
    for (j = 0; j < NUM_LEDS; j++)
    {
      NEO_setLEDColor(j, (red * i) / 255, (green * i) / 255, (blue * i) / 255);
    }
    NEO_sendLEDs();
    CHANGING();
    BTN_UNPRESSED();
    __delay_cycles(80000);
  }
  NEO_clearStrip();
  BY_CHANGE();
}

// 4. Animación "Construcción"
void buildUpAnimation(uint8 red, uint8 green, uint8 blue)
{
  uint16 i;
  for (i = 0; i < NUM_LEDS; i++)
  {
    NEO_setLEDColor(i, red, green, blue);
    NEO_sendLEDs();
    CHANGING();
    __delay_cycles(1200000);
  }
  NEO_clearStrip();
}
