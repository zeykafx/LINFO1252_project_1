# Projet 1 LINFO1252, groupe 17 Vendredi
- **Hugo Delporte**
- **Corentin Detry**

## Instructions
Pour lancer le projet, vous pouvez utiliser une des targets dans le makefile pour clean, compiler et lancer un problème avec un nombre prédéfinis de threads.
Sinon, vous pouvez nettoyer les fichiers compilés avec `make clean`, puis compiler tout avec `make` (ou `make all`), et puis lancer le programme avec `./bin/binary [ARGUMENTS HERE]`, pour la liste d'arguments disponible, voir ci-dessous.

Par exemple, pour lancer le problème des producers/consumers on peut faire : `make clean && make && ./bin/binary -c 4 -p 4` et on peut rajouter `-v` pour le mode verbose.

## Arguments possibles du programme
### Flags
- `-v`: active le mode verbose (désactivé si absent)
- `-o`: utilise les primitives de synchronisation de pthread (si absent, utilise nos primitives basées sur un lock test-and-test-and-set)
### Problèmes
#### Philosophes
- `-N [n_threads]`: lance le problème des philosophes avec `n_threads` threads (0 par default).
#### Producer consumer
- `-p [n_producer_threads] -c [n_consumer_threads]`: lance le problème des producer consumer avec `n_producer_threads` threads pour le producer et `n_consumer_threads` threads pour le consumer (0 par default pour les deux).
#### Reader Writer
- `-r [n_reader_threads] -w [n_writer_threads]`: lance le problème des reader writer avec `n_reader_threads` reader threads et `n_writer_threads` writer threaders (0 par default pour les deux).
#### Test-and-set & Test-and-test-and-set
- `-l [n_tas_threads]`: lance le programme de test du test-and-set lock avec `n_tas_threads` threads (0 par default).
- `-t [n_ttas_threads]`: lance le programme de test du test-and-test-and-set lock avec `n_ttas_threads` threads (0 par default).
#### Semaphores
- `-s [n_threads]`: lance le programme de test des semaphores avec `n_threads` threads.

## Makefile
Le makefile contient une série de targets qui permettent de lancer toutes sortes de choses
- `make clean` va nettoyer tous les fichiers compilés et les logs
- `make all` va compiler tous les fichiers `.c` se trouvant dans `./src` (et `main.c`) et place les fichiers `.o` dans `./lib`
  - Le fichier executable se trouve dans `./bin` et a le nom `binary`
- `make run` nettoie, compile et fait tourner le projet, mais sans arguments, vous pouvez ajouter des arguments dans la target
- `make valgrind` nettoie, compile et fait tourner valgrind sur le programme, vous devez mettre les arguments dans la target pour pouvoir inspecter la partie du projet que vous voulez
- `make philosophers` nettoie, compile et lance le programme avec les bons arguments pour lancer le problème des philosophes (avec 8 threads et mode verbose activé)
- `make producer_consumer` comme `make philosophers`, avec 4 producteurs et 4 consommateurs
- `make reader_writer` exactement comme `make producer_consumer`
- `make test_and_set` nettoie, compile et fait tourner le programme avec les arguments pour tester le spinlock test and set
- `make test_and_test_and_set` comme `make test_and_set` mais pour le spinlock test and test and set
- `make semaphore` nettoie, compile et lance le programme avec les arguments pour tester notre implémentation de semaphore (basé sur notre lock Test-and-test-and-set)
- `make graphs` lance le script de test de performance qui va tester tous les programmes avec nos locks/semaphores et celle de pthread, puis lance le script python pour générer les graphes (tous les graphes se trouvent dans `./data`)

## Organisation
Voici une brève description de l'organisation du projet.

- `./` dossier principale:
    - `./Makefile`: le Makefile contient toute les targets comme expliqué ci-dessus.
    - `./experiments.sh`: Le script de test de performance, va nettoyer puis compiler le programme, et par la suite faire tourner chaque problème 5 fois avec chaque configuration de threads dans 2, 4, 8, 16, 32, 64.
        - -> stocke les données dans `./data` en format csv, chaque ligne représente les 5 runs avec un certain nombre de threads (le nombre étant le premier entier sur la ligne), chaque case est donc les secondes.nanosecondes après execution et celui avant, le script python fait la soustraction pour obtenir la durée d'exécution
        - pour lancer les tests avec nos primitives de synchronisation, il faut lancer le script avec l'argument false, par exemple `./experiments.sh false`, pour lancer avec les mutex/locks de pthread, il faut lancer avec `./experiments.sh true`
    - `./analysis.py`: va générer les graphes sur base des données collectée par le script bash.
    - `./main.c`: fichier principal du programme, va parser les arguments et lancer le problème qui correspond aux arguments
- `./src`: dossier contenant tous les fichiers sources des différents problèmes
- `./headers`: dossier contenant tous les headers lié aux fichiers sources
- `./data`: contient tous les graphes et les données générés par le script bash et python
- `./bin`: contient le fichier executable produit par le makefile
- `./logs`: contient les logs de valgrind
