/**
   Copyright 2020 Marc SIBERT

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#pragma once

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

/**
 * Classe applicative générale.
 */
class Luciole {
public:

/**
 * Initialisation des pariphériques.
 */
  inline
  void begin() {
    DDRB |= _BV(PB0);  // Set PB0 as output
    initTimer0();
    initWatchdog();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  }

/**  
 * Boucle d'exécution.
 */
  inline
  void run() {
    while (true) {
      sleep_mode();
    }
  }
  
/**
 * Interruption du watchdog.
 */
  inline
  void watchdog() {
    wdt_reset();
    if (myRandom() > 30000) flash();
    sei();  // Pour être sûr !
    sleep_mode();
  }

protected:
/**
 * Produit un flash.
 */
  inline
  void flash() {
    TCCR0B = _BV(CS00); // Full speed
    for (byte i = 0; i < 32; ++i) {
      OCR0A = pgm_read_byte(CIEL8 + i);
      _delay_ms(5);
    }
    for (byte i = 31; i > 0; --i) {
      OCR0A = pgm_read_byte(CIEL8 + i);
      _delay_ms(50);
    }
    TCCR0B = 0;         // Timer disabled
  }

/**
 * Initialise le Timer 0 pour qu'il module le PORT B0 en PWM.
 */
  void initTimer0() {
    cli();
    TCCR0A = _BV(WGM01) | _BV(WGM00) |  // Mode 3 : Fast PWM (TOP = 0xFF)
             _BV(COM0A1);               // Clear on comp. match, set at BOTTOM
  //  TCCR0B = _BV(CS00); // Full speed
    TCCR0B = 0; // disabled
    TCNT0 = 0; // Reset counter
    OCR0A = 0; // compare match value
    sei();
  }

/**
 * Initialisation Watchdog.
 */
void initWatchdog() {
  cli();
  wdt_enable(WDTO_8S);
#if defined(__AVR_ATtiny85__)
  WDTCR |= _BV(WDIE) | _BV(WDE); 
#elif defined(__AVR_ATtiny13A__)
  WDTCR |= _BV(WDTIE) | _BV(WDE);
#else
  #error Pas d activation du watchdog !
#endif
  wdt_reset();
  sei();
}


private:
/**
 * Courbe log de l'éclairement (CIE Lightness loopup table function)
 * 5 bit CIE Lightness to 8 bit PWM conversion :
 *    L* = 116(Y/Yn)^1/3 - 16 , Y/Yn > 0.008856
 *    L* = 903.3(Y/Yn), Y/Yn <= 0.008856
 */
  static const uint8_t CIEL8[] PROGMEM;

/**
 * Générateur de nombres aléatoires sur 16 bits.
 * @return un entier non signé sur 16 bits.
 */
  unsigned myRandom() {
    static uint16_t x = 1;
    x ^= x << 7;
    x ^= x >> 9;
    return ( x ^= x << 8 );
  }

};

const uint8_t Luciole::CIEL8[] PROGMEM = {
  0,  1,  2,  3,  4,  5,  7,  9,  12, 15, 18, 22, 27, 32, 38, 44,
  51, 58, 67, 76, 86, 96, 108,120,134,148,163,180,197,216,235,255
};
