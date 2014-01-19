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

#include "luciole.h"

Luciole luciole;

ISR(WDT_vect) {
  luciole.wdt_vect();
}

void setup() {
  luciole.setup();
}

void loop() {
  luciole.loop();
}
