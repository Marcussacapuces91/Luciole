luciole
=======

Projet de luciole basé sur plusieurs projets dont une démo de <a href='https://twitter.com/asthrolab'>@asthrolab</a> rencontré à l'<a href='http://asthrolab.fr/2013/12/retour-sur-lopenbidouillecamp-2013-montreuil/'>OpenBidouilleCamp de Montreuil</a> (déc. 2013).

Bibliographie
-------------

* http://asthrolab.fr/2013/12/asthrolab-a-lopenbidouillecamp-montreuil/
* http://daviddarts.com/ledfirefly/

Matériel
--------

Je pars sur un micro-controleur Atmel Tiny 13 ou supérieur suivant les approvisionnements (le Tiny13 est en fin de vie), une diode électroluminescente, une pile et... rien d'autre.

<pre>
             -+ -+
             /| /|
            /  /
    vcc   | \ |                               vcc
     +----+---+-----+                          |
     |    | / |     |                       -------
     |              |                        #####    Pile (3v)
  +--+----+----+----+--+                       |     CR2023
  |                    |                    -------
   ) AtTiny 13/../85   |                     #####
  |.                   |                       |
  +--+----+----+----+--+                      gnd    
                    |   
                   gnd
</pre>

Fonctions
---------

- Une variation lente de l'éclairage et de l'extinction de la led (comme dans la démo d'asthrolab) ;
- Une période de scintillement aléatoire ;
- A améliorer : Une très faible consommation électrique du contrôleur.
- A faire : Utiliser la diode en capteur de luminosité pour ne faire briller la led que la nuit.

Pré-requis
----------
Comme je n'ai plus rien pour coder et flasher le microcontrôleur, je vais m'appuyer sur un arduino en mode ISP. Cela va me permettre d'utiliser :

1. l'IDE de l'Arduino pour saisir et compiler mon code ;
2. l'Arduino lui-même pour flasher le microcontroleur.


