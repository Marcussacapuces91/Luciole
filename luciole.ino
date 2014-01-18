/**
 * Luciole.ino
 * @author Marc Sibert - Copyright 2013
 *@licence Public Licence
 **/
 
#include <avr/sleep.h> 
#include <avr/wdt.h>
 

/**
 * Produit un flash de la luciole, une rampe montée progressive et une rampe descente.
 * @warning Cette fonction utilise le timer à son compte et empêche l'utilisation des fonctions sleep.
 **/
inline void flash() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  PRR &= ~_BV(PRTIM0);  // activation des alim timer0
  
  OCR0A = 0;               // Valeur initiale du PWM
  TCNT0 = 0;               // clear counter

  TCCR0A = _BV(COM0A1) | _BV(COM0A0) | // Clear on compare & set on TOP
           _BV(WGM01) | _BV(WGM00);    // Mode 3 Fast PWM
  TCCR0B = _BV(CS01);                  // no prescaling

// PB0 en sortie  
  DDRB |= _BV(DDB0); // | _BV(DDB1); 
  
  while (OCR0A < 254) {
    ++OCR0A;
    for (byte i = 4; i > 0; --i) {
      sleep_mode();
    }
  }
  while (OCR0A > 0) {
    --OCR0A;
    for (byte i = 8; i > 0; --i) {
      sleep_mode();
    }
  }
  
  TCCR0A = 0;  // On arrête le TIMER0 !
  
  DDRB &= ~_BV(DDB0); // Port B0 en entrée.
  PRR |= _BV(PRTIM0);  // desactivation de l'alim timer0
}

/**
 * Génère une attente aléatoire.
 * Cette fonction met le contrôleur en veille POWER DOWN et utilise l'interruption du watchdog
 * pour le réveiller au bout du temps de référence.
 **/
inline void wait() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  do {
// Activation du Watchdog
    cli();
    WDTCR = _BV(WDCE) | _BV(WDE);
    WDTCR = _BV(WDCE) | _BV(WDE) | WDTO_1S;
    MCUSR = 0;
    WDTCR |= _BV(WDIF) | _BV(WDIE);
    sei();

    wdt_reset();
// sleep until watchdog fire
    sleep_mode();

  } while (rand() > (RAND_MAX >> 4));
}

/**
 * Interruption lancé par le watchdog.
 **/
ISR(WDT_vect) {
// Désactivation du Watchdog
  cli();
  wdt_disable();
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;
  sei();
}

/**
 * Fonction d'initialisation du contrôleur.
 * Elle configure les bases de temps suivant les besoins et désactive les fonctions inutilisées.
 **/
void setup() {
#ifdef __AVR_ATtiny13__
  cli();
  CLKPR = _BV(CLKPCE);  
  CLKPR = 0b0011;  // pas plus de 8 en 128 kHz !!!
  sei();
#endif
#ifdef __AVR_ATtiny85__
#endif

// Désactivation du Watchdog
  cli();
  wdt_disable();
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;
  sei();
  
// Reduc power analog
  ADCSRA &= ~_BV(ADEN);  //  ADC switched off
  ACSR = _BV(ACD);       // Analog Comparator Disable
  DIDR0 = 0x3F;          // Digital Input Disable (toutes)

// Reduc power by disabled clock signal (en dernier)
  PRR = 0x0F;    
}


/**
 * Fonction itérative du contrôleur.
 * Elle alterne une séquence de d'éclairement de la led et une séquence d'attente infiniement.
 **/
void loop() {
  flash();
  wait(); 
}
