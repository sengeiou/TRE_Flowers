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

#include "Snake.h"
#include "Sprites.h"

// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Classe d'un bloc du serpent
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructeurs
// ------------------------------------------------------------------------------------------------------------------------------------------------------
SnakePart::SnakePart() { }

SnakePart::SnakePart(uint8_t positionX, uint8_t positionY) {

  x = positionX;
  y = positionY;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Classe du serpent
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructeur
// ------------------------------------------------------------------------------------------------------------------------------------------------------
Snake::Snake() { }

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Initialisation du serpent
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::init(Arduboy *adb, uint8_t gameState) { 

  direction = SNAKEDIR_RIGHT;

  foodEaten = 0;
  
  isDead = false;
 
  arduboy = adb;

  // Initialisation du serpent pour le jeu
  if (gameState == GAMESTATE_GAME) {

    x = 72;
    y = 32;
    
    partsCount = 5;
    partsStart = 0;
    parts[0] = SnakePart(56, 32);
    parts[1] = SnakePart(60, 32);
    parts[2] = SnakePart(64, 32);
    parts[3] = SnakePart(68, 32);
    parts[4] = SnakePart(72, 32);
  
    speed = 2;
    arduboy->setFrameRate(speed);
  
    food.init(arduboy);
  
    food.generate();
  }

  // Initialisation du serpent pour le menu principal
  else {

    x = 96;
    y = 0;   
    
    partsCount = 25;
    partsStart = 0;

    for (uint8_t i=0; i<partsCount; i++) {
      
      parts[i] = SnakePart(i * SNAKE_PART_SIZE, 0);
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Déplacement du serpent dans le jeu
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::move() { 

  // Mise à jour de la position du serpent en fonction de son orientation 
  if (direction == SNAKEDIR_UP) {
  
    y -= SNAKE_PART_SIZE;
  }
  else if (direction == SNAKEDIR_DOWN) {
  
    y += SNAKE_PART_SIZE;
  }
  else if (direction == SNAKEDIR_LEFT) {
  
    x -= SNAKE_PART_SIZE;
  }
  else {
  
    x += SNAKE_PART_SIZE;
  }

  if (checkCollisionWithItself() || checkCollisionWithBorder()) {
    
    isDead = true;

    // Génération d'un son de défaite si le son est activé
    if (canMakeSound) {
      
//      arduboy->tunes.tone(1100, 80);
//      delay(100);
//      arduboy->tunes.tone(1000, 80);
//      delay(100);
//      arduboy->tunes.tone(500, 500);
    }
  }

  if (checkCollisionWithFood()) {
  
    // Grossissement du serpent 
    parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
    partsCount++;
    
    // Augmentation du nombre de repas mangés par le serpent
    foodEaten++;

     // Mise à jour de la vitesse du jeu en fonction du score
    if (foodEaten % 3 == 0 && speed < 13) {

      speed++;
      if (speed == 3) speed++; // Bug avec "setFrameRate" avec une valeur de 3.
      arduboy->setFrameRate(speed);
    }
  
    // Génération d’un nouveau repas
    food.generate();

    // Génération d'un son de victoire si le son est activé
    if (canMakeSound) {
      
//      arduboy->tunes.tone(500, 50);
//      delay(80);
//      arduboy->tunes.tone(800, 50);
//      delay(80);
//      arduboy->tunes.tone(1100, 100);
    }
  }
  else {
  
    // Déplacement du serpent
    parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
    partsStart = (partsStart + 1) % MAX_SNAKE_PARTS;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Déplacement du serpent sur le menu principal
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::moveOnMainMenu() { 

  uint8_t randomDir = 0;

  // Déplacement du serpent en fonction d'où il vient et d'où il se trouve
  if (x == 124 && y == 0) {

    // Si il vient 
    if (direction == SNAKEDIR_RIGHT) {
      
      direction = SNAKEDIR_DOWN;
      y += SNAKE_PART_SIZE;
    }
    else {
      
      direction = SNAKEDIR_LEFT;
       x -= SNAKE_PART_SIZE;
    }
  }
  else if (x == 124 && y == 32) {
    
    if (direction == SNAKEDIR_RIGHT) {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
    }
    else if (direction == SNAKEDIR_DOWN) {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_LEFT;
        x -= SNAKE_PART_SIZE;
      }
    }
    else {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_LEFT;
        x -= SNAKE_PART_SIZE;
      }
    }
  }
  else if (x == 124 && y == 60) {
    
    if (direction == SNAKEDIR_DOWN) {
      
      direction = SNAKEDIR_LEFT;
      x -= SNAKE_PART_SIZE;
    }
    else {
     
      direction = SNAKEDIR_UP;
      y -= SNAKE_PART_SIZE;
    }
  }
  else if (x == 0 && y == 60) {
    
    if (direction == SNAKEDIR_LEFT) {
      
      direction = SNAKEDIR_UP;
      y -= SNAKE_PART_SIZE;
    }
    else {
     
      direction = SNAKEDIR_RIGHT;
      x += SNAKE_PART_SIZE;
    }
  }
  else if (x == 0 && y == 32) {
    
    if (direction == SNAKEDIR_LEFT) {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
    }
    else if (direction == SNAKEDIR_DOWN) {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_DOWN;
        y += SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_RIGHT;
        x += SNAKE_PART_SIZE;
      }
    }
    else {

      randomDir = random(0, 2);

      if (randomDir) {
      
        direction = SNAKEDIR_UP;
        y -= SNAKE_PART_SIZE;
      }
      else {
        
        direction = SNAKEDIR_RIGHT;
        x += SNAKE_PART_SIZE;
      }
    }
  } 
  else if (x == 0 && y == 00) {
    
    if (direction == SNAKEDIR_LEFT) {
      
      direction = SNAKEDIR_DOWN;
      y += SNAKE_PART_SIZE;
    }
    else {
     
      direction = SNAKEDIR_RIGHT;
      x += SNAKE_PART_SIZE;
    }
  }
  else {

    if (direction == SNAKEDIR_UP) {
    
      y -= SNAKE_PART_SIZE;
    }
    else if (direction == SNAKEDIR_DOWN) {
    
      y += SNAKE_PART_SIZE;
    }
    else if (direction == SNAKEDIR_LEFT) {
    
      x -= SNAKE_PART_SIZE;
    }
    else {
    
      x += SNAKE_PART_SIZE;
    }
  }
  
  // Déplacement du serpent
  parts[(partsStart + partsCount) % MAX_SNAKE_PARTS] = SnakePart(x, y);
  partsStart = (partsStart + 1) % MAX_SNAKE_PARTS;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Affichage du serpent à l'écran
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void Snake::draw() {

  uint8_t currentPart; // Bloc du serpent en cours de traitement

  // Affichage de chaque bloc du serpent
  for (uint8_t i=0; i<partsCount; i++) {

    currentPart = (partsStart + i) % MAX_SNAKE_PARTS;
    
    // Si le bloc en cours est celui de la tête
    if (i == (partsCount - 1)) {

      // Affichage du bloc de la tête en fonction de son orientation
      if (direction == SNAKEDIR_UP) {
        
        arduboy->drawBitmap(parts[currentPart].x, parts[currentPart].y, snakeHeadUp, SNAKE_PART_SIZE, SNAKE_PART_SIZE, 1);
      }
      else if (direction == SNAKEDIR_DOWN) {
    
        arduboy->drawBitmap(parts[currentPart].x, parts[currentPart].y, snakeHeadDown, SNAKE_PART_SIZE, SNAKE_PART_SIZE, 1);
      }
      else if (direction == SNAKEDIR_LEFT) {
    
        arduboy->drawBitmap(parts[currentPart].x, parts[currentPart].y, snakeHeadLeft, SNAKE_PART_SIZE, SNAKE_PART_SIZE, 1);
      }
      else {
        
        arduboy->drawBitmap(parts[currentPart].x, parts[currentPart].y, snakeHeadRight, SNAKE_PART_SIZE, SNAKE_PART_SIZE, 1);
      }
    } 
    
    // Si le bloc en cours n'est pas celui de la tête
    else {

      // Affichage du bloc du corps
      arduboy->drawBitmap(parts[currentPart].x, parts[currentPart].y, snakeBody, SNAKE_PART_SIZE, SNAKE_PART_SIZE, 1);
    }
  } 
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Test de collision du serpent avec la nourriture
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithFood() { 
  
  if (x == food.x && y == food.y) return true;
  else return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Test de collision du serpent avec la bordure de l'écran
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithBorder() { 
  
  if (x == 0 || x == 124 || y == 0 || y == 60) return true;
  else return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Test de collision du serpent avec lui même
// ------------------------------------------------------------------------------------------------------------------------------------------------------
bool Snake::checkCollisionWithItself() { 

  uint8_t currentPart; // Bloc du serpent en cours de traitement

  // Contrôle de chaque bloc du serpent
  for (uint8_t i=0; i<partsCount; i++) {

    currentPart = (partsStart + i) % MAX_SNAKE_PARTS;

    if (x == parts[currentPart].x && y == parts[currentPart].y) {
      
      return true;
    }
  }
  
  return false;
}
