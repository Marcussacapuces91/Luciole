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
#include <util/delay.h>

#undef DEBUG

enum proc_t {
  AVR_ATtiny13A,
  AVR_ATtiny85,
  AVR_ATmega328P 
};

/**
 * Classe applicative générale.
 */
template<proc_t PROC>
class Luciole {
public:

/**
 * Initialisation des pariphériques.
 */
  void begin() {
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) ;
    Serial.println("Init");
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    initTimer0();
    initWatchdog();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    setAnode(true);             // OUTPUT, LOW (default)
    flash();
  }

/**  
 * Boucle d'exécution.
 */
  void run() {
#ifdef DEBUG
    Serial.println("Run");
#endif
    while (true) {
      if (testLum() && (myRandom() > 40000)) flash();
#ifdef DEBUG
      Serial.print("Sleep...");
      Serial.flush();
#endif
      sleep_mode();
#ifdef DEBUG
      Serial.println("wake up");
      Serial.flush();
#endif
    }
  }
  
/**
 * Interruption du watchdog.
 * Juste pour réveiller la CPU.
 */
  void watchdog() {}

protected:
/**
 * Produit un flash sur sortie A du timer0 (OC0A)
 */
  void flash() {
    setCathode(true, false); // OUTPUT, LOW
    TCNT0 = 0; // Reset counter
    TCCR0B = _BV(CS00); // Full speed
    for (byte i = 0; i < 32; ++i) {
      OCR0A = pgm_read_byte(CIEL8 + i);
      _delay_ms(10);
    }
    for (unsigned i = 8*31; i > 0; --i) {
      OCR0A = pgm_read_byte(CIEL8 + (((i ^ 0x01) + 3) / 8));  // bits scrambling
      _delay_ms(10);
    }
    TCCR0B = 0;         // Timer disabled
  }

  bool testLum() {
    setCathode(false, true); // INPUT PULLUP
    _delay_ms(10);
    setCathode(false, false); // INPUT /PULLUP
    _delay_ms(500);
    return readCathode();
  }

/**
 * Initialise le Timer 0 pour qu'il module le PORT B0 en PWM.
 */
  void initTimer0() {
    cli();
    TCCR0A = _BV(WGM01) | _BV(WGM00) |  // Mode 3 : Fast PWM (TOP = 0xFF)
             _BV(COM0A1);               // Clear on comp. match, set at BOTTOM
    TCCR0B = 0; // disabled
    TCNT0 = 0; // Reset counter
    OCR0A = 0; // compare match value
    sei();
  }

/**
 * Initialisation Watchdog.
 */
void initWatchdog();

/*
{
  cli();
  wdt_reset();
  MCUSR &= ~_BV(WDRF);

// Delay 8 sec + Interrupt Mode (only)
#if defined(__AVR_ATtiny85__)
  wdt_enable(WDTO_8S);
  WDTCR |= _BV(WDIE) | _BV(WDE); 
#elif defined(__AVR_ATtiny13A__)
  wdt_enable(WDTO_8S);
  WDTCR |= _BV(WDTIE) | _BV(WDE);
#elif defined(__AVR_ATmega328P__)
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDIE) | _BV(WDP3) | _BV(WDP0); // Interrupt + 8 sec
#else
  #error Pas d activation du watchdog !
#endif
  sei();
}
*/

private:
  inline
  void setAnode(const bool state = true, const bool value = false);

  inline
  void setCathode(const bool state = true, const bool value = false);

  inline
  bool readCathode() const;
  
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
    static unsigned x = 1;
    x ^= x << 7;
    x ^= x >> 9;
    return ( x ^= x << 8 );
  }

};

template <proc_t PROC>
const uint8_t Luciole<PROC>::CIEL8[] PROGMEM = {
  0,  1,  2,  3,  4,  5,  7,  9,  12, 15, 18, 22, 27, 32, 38, 44,
  51, 58, 67, 76, 86, 96, 108,120,134,148,163,180,197,216,235,255
};

#ifdef __AVR_ATmega328P__

template <>
void Luciole<AVR_ATmega328P>::initWatchdog() {
  cli();
  wdt_reset();
  MCUSR &= ~_BV(WDRF);
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDIE) | _BV(WDP3) | _BV(WDP0); // Interrupt + 8 sec
  sei();
}

template <>
void Luciole<AVR_ATmega328P>::setAnode(const bool state, const bool value) {
    DDRD = state ? DDRD | _BV(PD6) : DDRD & ~_BV(PD6);
    PORTD = value ? PORTD | _BV(PD6) : PORTD & ~_BV(PD6);
}

template <>
void Luciole<AVR_ATmega328P>::setCathode(const bool state, const bool value) {
    DDRD = state ? DDRD | _BV(PD7) : DDRD & ~_BV(PD7);
    PORTD = value ? PORTD | _BV(PD7) : PORTD & ~_BV(PD7);
}

template <>
bool Luciole<AVR_ATmega328P>::readCathode() const {
  return PIND & _BV(PD7);
}

#endif

#ifdef __AVR_ATtiny85__

template <>
void Luciole<AVR_ATtiny85>::initWatchdog() {
  cli();
  wdt_reset();
  MCUSR &= ~_BV(WDRF);
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = _BV(WDIE) | _BV(WDP3) | _BV(WDP0); // Interrupt + 8 sec
  sei();
}

template <>
void Luciole<AVR_ATtiny85>::setAnode(const bool state, const bool value) {
    DDRB = state ? DDRB | _BV(PB0) : DDRB & ~_BV(PB0);
    PORTB = value ? PORTB | _BV(PB0) : PORTB & ~_BV(PB0);
}

template <>
void Luciole<AVR_ATtiny85>::setCathode(const bool state, const bool value) {
    DDRB = state ? DDRB | _BV(PB2) : DDRB & ~_BV(PB2);
    PORTB = value ? PORTB | _BV(PB2) : PORTB & ~_BV(PB2);
}

template <>
bool Luciole<AVR_ATtiny85>::readCathode() const {
  return PINB & _BV(PB2);
}

#endif
