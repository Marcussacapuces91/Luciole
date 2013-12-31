/*
  Luciole.ino
@author Marc Sibert - Copyright 2013
@licence Public Licence
 */
 
#include <avr/sleep.h> 
#include <avr/wdt.h>
 

#define PENTE 1

/**
 * Produit un flash de la luciole, une rampe montée et une rampe descente.
 * @warning Cette fonction utilise le timer à son compte et empêche l'utilisation des fonctions sleep.
 * @warning Elle restaure toutefois proprement l'état du timer après son utilisation.
 */
inline void flash() {
  
  const uint8_t oldTCCR0A = TCCR0A;
  const uint8_t oldTCCR0B = TCCR0B;
  const uint8_t oldOCR0A = OCR0A;
  
  
  OCR0A = 0;               // Valeur initiale du PWM
  TCNT0 = 0;               // clear counter

  TCCR0A = _BV(COM0A1) | _BV(COM0A0) | // Clear on compare & set on TOP
           3;              // Mode 3 Fast PWM
  TCCR0B = 1;_BV(CS01) | _BV(CS00);    // div 64 

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

  TCCR0A = oldTCCR0A;  // OC0A & OC0B déconnectés et Mode normal.
  TCCR0B = oldTCCR0B;  // No clock source (Timer/Counter stopped)
  OCR0A = oldOCR0A;
  
}

void setup() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  cli();
  CLKPR = _BV(CLKPCE);  
  CLKPR = 0b0011;  // pas plus de 8 en 128 kHz !!!
  sei();
}

void loop() {
  flash();

// prépare le timer à 1 sec 
// f = 128 khz
// CKDIV8 quel effet ???

// Normal mode sans compare ==> TOV0
  TIMSK0 = _BV(TOIE0);  // ne garder que TOverflow et masquer les autres
  GTCCR = _BV(PSR10);   // reset prescaler
  TCNT0 = 0;            // clear counter
  TCCR0A = 0b00000000;
  TCCR0B = 0b00000101;  
  
  do {
    sleep_mode();
  } while (rand() > (RAND_MAX >> 6));
}
