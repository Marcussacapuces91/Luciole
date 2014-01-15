/*
  Luciole.ino
@author Marc Sibert - Copyright 2013
@licence Public Licence
 */
 
#include <avr/sleep.h> 
#include <avr/wdt.h>
 

/**
 * Produit un flash de la luciole, une rampe montée et une rampe descente.
 * @warning Cette fonction utilise le timer à son compte et empêche l'utilisation des fonctions sleep.
 */
inline void flash() {
  
  OCR0A = 0;               // Valeur initiale du PWM
  TCNT0 = 0;               // clear counter

  TCCR0A = _BV(COM0A1) | _BV(COM0A0) | // Clear on compare & set on TOP
           _BV(WGM01) | _BV(WGM00);    // Mode 3 Fast PWM
  TCCR0B = _BV(CS01); // | _BV(CS00);    // div 64 

// PB0 en sortie  
  DDRB |= _BV(DDB0); // | _BV(DDB1); 
  
  while (OCR0A < 254) {
    OCR0A += 2;
    sleep_mode();
  }
  while (OCR0A > 0) {
    OCR0A -= 1;
    sleep_mode();
  }
  
  DDRB &= ~_BV(DDB0); // Port B0 en entrée.

}

void on(const int d) {
  on();
  delay(d);
  off();
}

inline void on() {
  PORTB |= _BV(PB4);
  DDRB |= _BV(DDB4);
}

inline void off() {
  PORTB &= ~_BV(PB4);
  DDRB &= ~_BV(DDB4);
}

void setup() {
  set_sleep_mode(SLEEP_MODE_IDLE);
#ifdef __AVR_ATtiny13__
  cli();
  CLKPR = _BV(CLKPCE);  
  CLKPR = 0b0011;  // pas plus de 8 en 128 kHz !!!
  sei();
#endif
#ifdef __AVR_ATtiny85__
// Mettre l'horloge en 128KHz interne avec lfuse=0xE4
/*
  cli();
  CLKPR = _BV(CLKPCE); // CLKPCE: Clock Prescaler Change Enable
  CLKPR = 0b0110;      //  CLKPS[3:0]: Clock Prescaler Select Bits 3 - 0 : pas plus de 8 en 128 kHz !!!
  sei();
*/
#endif
}

void loop() {
  flash();
 
// Normal mode sans compare ==> TOV0
#ifdef __AVR_ATtiny13__
  TIMSK0 = _BV(TOIE0);  // ne garder que TOverflow et masquer les autres
  GTCCR = _BV(PSR10);   // reset prescaler
#endif
#ifdef __AVR_ATtiny85__
  TIMSK = _BV(TOIE0);   // ne garder que TOverflow et masquer les autres
  GTCCR = _BV(PSR0);    // reset prescaler
#endif

  TCNT0 = 0;            // clear counter
  TCCR0A = 0b00000000;  // Mode normal : un TOV à la fin du comptage
  TCCR0B = _BV(CS02) | _BV(CS00);  // 101 : clkI/O / 1024 (From prescaler)


  do {
    sleep_mode();
//    on(1);
  } while (rand() > (RAND_MAX >> 3));
  
  
}
