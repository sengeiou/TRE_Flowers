//
// ArduSnake : Un jeu de Snake créé pour le numéro 198 du magazine "Programmez!" 
//
// Copyright (C) 2016 Jerome Perrot (Initgraph)
//
// Version : 1.0 :
//
//   - Ajout d'un menu principal.
//   - Possibilité de mettre en pause le jeu en appuyant sur le bouton B pendant la partie.
//   - Possibilité de couper le son en appuyant sur le bouton A pendant la partie.
//   - Augmentation de la vitesse du serpent au fur et à mesure qu'il mange.
//   - Ajout d'une tête au serpent.
//   - Ajout d'un meilleur score avec sauvegarde sur l'EEPROM.
//
// Notes :
//
//   Il est nécéssaire d'installer la bibliothèque de développement de l'Arduboy pour compiler ce programme, plus d'infos sont disponibles ici :
//   
//   https://github.com/arduboy/arduboy
// 
// Licence :  
//
//   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc.,
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Pour toutes remarques, suggestions ou questions, vous pouvez me contacter à l'adresse contact@initgraph.com ou via Twitter : @InitgraphDotCom
// Vous pouvez également visiter mon site web : http://www.initgraph.com 
//
// Amusez vous bien !
//

#ifndef SNAKE_H
#define SNAKE_H

#include "Constants.h"
#include "Food.h"

#ifdef __cplusplus
extern "C" {
#endif
	
	#include "arduboy.h"

#ifdef __cplusplus
}
#endif

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Classe d'un bloc du serpent
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class SnakePart {
  
  public:

    uint8_t x;
    uint8_t y;

    SnakePart(); 
    SnakePart(uint8_t positionX, uint8_t positionY);
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Classe du serpent
// ------------------------------------------------------------------------------------------------------------------------------------------------------
class Snake {
  
  public:
  
    uint8_t x;
    uint8_t y; 
    uint8_t direction;
    Food food;
    uint8_t foodEaten;
    bool isDead;
    uint8_t speed;
    bool canMakeSound;

    Snake();
    void init(Arduboy *adb, uint8_t gameState);
    void move();
    void moveOnMainMenu();
    void draw();
    
  private:

    uint8_t partsCount; 
    uint8_t partsStart;
    SnakePart parts[MAX_SNAKE_PARTS];
    Arduboy *arduboy;

    bool checkCollisionWithFood();
    bool checkCollisionWithBorder();
    bool checkCollisionWithItself();
};

#endif
