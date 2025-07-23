
# Projet Net - Groupe B31

## Description

Bienvenue dans notre projet du jeu **"Le Net"** !  
Il s'agit d'un jeu de puzzle. Dans une grille (5x5 par défaut) se trouvent des pièces que l'on peut tourner d'un quart de tour dans le sens horaire ou antihoraire.  
Chaque pièce est définie par sa forme (cul-de-sac, segment, coin, T ou croix) et son orientation (N, E, S, W).  
Le but du jeu est de connecter l'ensemble des pièces pour former un **Net** (réseau).

Vous pouvez jouer au jeu de trois façons différentes :
- [Console](#game_text)
- [Interface graphique (SDL)](#game_sdl)
- [Version web](#version-web) *(à venir)*

---

## La bibliothèque

Les fonctions utiles au bon fonctionnement du jeu sont définies dans les modules **`game`**, **`game_aux`**, **`game_ext`** et **`game_tools`**.  
Les fonctions propres à l'interface graphique sont définies dans le module **`model`**.  
La définition d'un **jeu** se trouve dans le fichier **`game_struct.h`**.

Pour plus d'informations sur les fonctions du jeu, consultez la [documentation officielle du jeu](https://pt2.pages.emi.u-bordeaux.fr/support/doc/v2/html/).  
Vous pouvez également tester la [version originale ici](https://www.chiark.greenend.org.uk/~sgtatham/puzzles/js/net.html).

---

## Les exécutables

### game_text

**game_text** est la version **console** du jeu, entièrement en **mode texte**.  
Utilisation simple :

```sh
./game_text [<filename>]
```

- L'argument `<filename>` permet de charger une partie depuis un fichier (facultatif).  
- Si aucun fichier n'est fourni, le jeu par défaut sera chargé.  
- Tapez la commande `h` durant le jeu pour ouvrir le menu d'aide.

---

### game_random

Cet exécutable permet de générer un jeu aléatoire avec des paramètres définis.  
Il attend au moins **6 arguments** :

1. Nombre de colonnes
2. Nombre de lignes
3. Topologie torique (0 ou 1)
4. Nombre de cases vides
5. Nombre d'arêtes supplémentaires
6. Mélange du jeu (0 ou 1)

Un septième argument optionnel permet de sauvegarder dans un fichier.

Commande type :

```sh
./game_random <nb_cols> <nb_rows> <wrapping> <nb_empty> <nb_extra> <shuffle> [<filename>]
```

**Exemple** :

```sh
./game_random 6 5 1 3 2 0 random.sol
```

---

### game_solve

**game_solve** propose deux options :

- `-s` : Chercher une solution à un jeu
- `-c` : Compter le nombre de solutions possibles

Utilisation :

```sh
./game_solve <option> <input> [<output>]
```

- `<input>` est le fichier d'entrée.
- `<output>` est facultatif et permet de sauvegarder le résultat.

**Exemple** :

```sh
./game_solve -s default.txt default_sol.txt
```

---

### game_test

Trois exécutables de tests sont disponibles :

- `game_test_ddausse`
- `game_test_eucer`
- `game_test_hbotanlioglu`

Utilisation :

```sh
./game_test_<name> <testname>
```

**Exemple** :

```sh
./game_test_ddausse game_print
```

---

### game_sdl

**game_sdl** est la version **graphique** du jeu utilisant **SDL**.  
L'utilisation est similaire à **game_text** :

```sh
./game_sdl [<filename>]
```

---

## Version Web

WIP

---

## Arborescence du projet

```plaintext
net-b31
├── CMakeLists.txt
├── default.txt
├── README.md
├── res
│   ├── background.png
│   ├── corner.png
│   ├── cross.png
│   ├── endpoint.png
│   ├── Quicksand-Regular.ttf
│   ├── segment.png
│   └── tee.png
├── src
│   ├── game_aux.c
│   ├── game_aux.h
│   ├── game.c
│   ├── game_ext.c
│   ├── game_ext.h
│   ├── game.h
│   ├── game_private.c
│   ├── game_private.h
│   ├── game_random.c
│   ├── game_sdl.c
│   ├── game_solve.c
│   ├── game_struct.h
│   ├── game_text.c
│   ├── game_tools.c
│   ├── game_tools.h
│   ├── model.c
│   ├── model.h
│   ├── queue.c
│   └── queue.h
└── tests
    ├── game_test_ddausse.c
    ├── game_test_eucer.c
    └── game_test_hbotanlioglu.c
```

---

## Commandes utiles

À exécuter depuis le dossier `net-b31` :

```sh
# Supprimer et recréer le dossier build
rm -rf build
mkdir build
cd build

# Générer les fichiers de build avec CMake (RELEASE par défaut)
cmake ..
# Pour générer en mode DEBUG
cmake -DCMAKE_BUILD_TYPE=DEBUG ..

# Compiler le projet
make

# Exécuter les tests
make test
ctest

# Vérifications additionnelles (mode DEBUG)
make ExperimentalCoverage
make ExperimentalMemCheck
```

---

## Auteurs

- Dorian DAUSSE
- Ege UCER
- Hasan Tugberk BOTANLIOGLU

---