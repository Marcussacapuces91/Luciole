/**
* @package     Luciole
* @author      Marc SIBERT
* @contributor
* @copyright   Copyright © 2014 Marc Sibert <marc@sibert.fr>
* @link        https://github.com/Marcussacapuces91/Luciole
* @licence     This work is free. You can redistribute it and/or modify it under the terms of the Do What The Fuck You Want To Public License, Version 2, as published by Sam Hocevar. See the COPYING file for more details.
*/

/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */
 
#ifndef __LUCIOLE_H__
#define __LUCIOLE_H__

#include <Arduino.h>
#include <avr/sleep.h> 
#include <avr/wdt.h>

/**
 * Classe avec des méthodes statiques pour éviter la multiinstanciation et optimiser le code généré.
 */
class Luciole {
  protected:
/// Temps de montée.
    static const unsigned MONTEE = 3;

/// Temps de descente.
    static const unsigned DESCENTE = 12;
    
// Probailité invert (1/p) d'arrêter l'attente chaque seconde.
    static const unsigned INV_PROBA = 16;
  
  public:
/**
 * Fonction d'initialisation du contrôleur ; elle doit être appelée par la fonction setup() du sketch Arduino.
 * Elle désactive les fonctions inutilisées.
 */
    static void setup() {
// Désactivation du Watchdog (au cas où)
      wdt_disable();  
      
// Reduc power analog
      ADCSRA &= ~_BV(ADEN);  //  ADC switched off
      ACSR = _BV(ACD);       // Analog Comparator Disable
      DIDR0 = 0x3F;          // Digital Input Disable (toutes)
    
// Reduc power by disabled clock signal (en dernier)
    #ifdef __AVR_ATtiny85__
      PRR = 0x0F;    
    #endif
    }

/**
 * Fonction itérative du contrôleur ; elle doit être appelée par la fonction loop() du sketch Arduino.
 * Elle assure une séquence de d'éclairement de la led et une séquence d'attente.
 */
    static void loop() {
      flash();
      wait();
    }  
    
/**
 * Interruption du watchdos ; elle doit être appelée par l'interruption ISR(WDT_vect()) du sketch Arduino.
 * Elle ne fait rien, mais impose la génération d'un code vide pour l'interruption du Watchdog.
 */
    static void wdt_vect() {}
    
  protected:

/**
 * Cette méthode assure l'éclairement et l'exctinction progressive de la led.
 * Elle utilise le mode de sommeil "IDLE" qui ne n'arrête que la CPU.
 * Elle utilise le timer 0 afin de générer à la fois le signal de sortie PWM sur le port PB0,
 * mais aussi pour produire les interruptions de réveille de la CPU (en fin de cycle PWM : TOP).
 * @warning : la sortie sur PB0 est "inversée", c'est à dire que la led doit être connectée entre Vcc et PB0.
 */
    static void flash() {
      set_sleep_mode(SLEEP_MODE_IDLE);
#ifdef __AVR_ATtiny85__
      PRR &= ~_BV(PRTIM0);  // activation des alim timer0
#endif
    
      OCR0A = 0;               // Valeur initiale du PWM
      TCNT0 = 0;               // clear counter
    
      TCCR0A = _BV(COM0A1) | _BV(COM0A0) | // Clear on compare & set on TOP
               _BV(WGM01) | _BV(WGM00);    // Mode 3 Fast PWM
      TCCR0B = _BV(CS01);                  // no prescaling
    
    // PB0 en sortie  
      DDRB |= _BV(DDB0); // | _BV(DDB1); 
      
      while (OCR0A < 254) {
        ++OCR0A;
        for (byte i = MONTEE * 1000000 / F_CPU; i > 0; --i) {
          sleep_mode();
        }
      }
      while (OCR0A > 0) {
        --OCR0A;
        for (byte i = DESCENTE * 1000000 / F_CPU ; i > 0; --i) {
          sleep_mode();
        }
      }
      
      TCCR0A = 0;  // On arrête le TIMER0 !
      
      DDRB &= ~_BV(DDB0); // Port B0 en entrée.
#ifdef __AVR_ATtiny85__
      PRR |= _BV(PRTIM0);  // desactivation de l'alim timer0
#endif
    }  
    
/**
 * Cette méthode assure une pause d'une durée aléatoire.
 * Elle utilise le mode de sommeil "POWER_DOWN" qui arrête toutes les fonctions sauf le Watchdog.
 * Elle utilise donc le Watchdog comme base de temps pour assurer les réveil du contrôleur et le test de la fin de la durée aléatoire.
 */
    static void wait() {
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    
// Activation du Watchdog
      wdt_enable(WDTO_1S);  // Période de 1 sec
      cli();                // Définition du mode Interrupt
      WDTCR |= _BV(WDCE) | _BV(WDE);  // Séq. Enable Change
#ifdef __AVR_ATtiny13__
      WDTCR = (WDTCR | _BV(WDTIE)) & ~_BV(WDE);  // set WDTIE & clear WDE
#endif
#ifdef __AVR_ATtiny85__
      WDTCR = (WDTCR | _BV(WDIE)) & ~_BV(WDE);  // set WDIE & clear WDE
#endif
      sei();
      
      do {
        wdt_reset();
        sleep_mode();  // sleep until watchdog fire
      } while (rand() > (RAND_MAX / INV_PROBA));
      wdt_disable();
    }
};

#endif // __LUCIOLE_H__
