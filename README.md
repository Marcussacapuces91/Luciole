Luciole (V2)
============

Projet de luciole basé sur plusieurs projets dont une démo de <a href='https://twitter.com/asthrolab'>@asthrolab</a> rencontré à l'<a href='http://asthrolab.fr/2013/12/retour-sur-lopenbidouillecamp-2013-montreuil/'>OpenBidouilleCamp de Montreuil</a> (déc. 2013).

Bibliographie
-------------

- http://asthrolab.fr/2013/12/asthrolab-a-lopenbidouillecamp-montreuil/
- http://daviddarts.com/ledfirefly/ (n'existe plus) - https://vimeo.com/66512233

Matériel
--------

Je pars sur un micro-contrôleur Atmel ATtiny 13 ou supérieur (ATtiny 85)suivant les approvisionnements (le Tiny13 est en fin de vie), une diode électroluminescente, une pile et... rien d'autre.

<pre>
    vcc                                          vcc
     |              +-------+                     |
  +--+----+----+----+--+    |                  -------
  |                    |  --+--                 #####   Pile (3v)
   ) AtTiny 13/../85   |   \ /  ---->             |     CR2023
  |.                   |   -+-  ---->          -------
  +--+----+----+----+--+    |                   #####
                    +-------+                     |
                   gnd                           gnd
</pre>

Fonctions
---------

- Une variation lente de l'éclairage et de l'extinction de la LED (comme dans la démo d'Asthrolab) ;
- Une période de scintillement aléatoire ;
- Une très faible consommation électrique du contrôleur (en utilisant les fonctions de mise en sommeil) ;
- À faire : Utiliser la diode en capteur de luminosité pour ne faire briller la LED que la nuit.

Pré-requis
----------
Comme je n'ai plus rien pour coder et flasher le microcontrôleur, je vais m'appuyer sur un Arduino en mode ISP. Cela va me permettre d'utiliser :

1. l'IDE de l'Arduino pour saisir et compiler mon code ;
2. l'Arduino lui-même pour flasher le microcontrôleur.

Licence
-------
La seconde itération de ce code est placé sous <a href="LICENSE-2.0">licence Apache V2</a>.

