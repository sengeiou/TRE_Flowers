#include "SnakePort.h"
#include "Snake.h"
#include "stdio.h"
#include <string>
using namespace std;

Snake snake;
static uint8_t gameState;             // Etat du jeu à afficher 
char scoreText[4];             // Score à afficher
uint8_t highScore = 0;         // Meilleur score
char highScoreText[4];         // Meilleur score à afficher 
uint8_t blinkingTextState = 0; // Permet de gérer un texte clignotant

void snake_init()
{
	arduboy.begin();
	
  // Le menu principal est affiché en premier
  gameState = GAMESTATE_MAINMENU;
  snake.init(&arduboy, GAMESTATE_MAINMENU);
}

void snake_loop()
{
	string msg = "";
  // Si le serpent est mort, un écran « GAME OVER » est affiché
  if (snake.isDead) {
    
    // Ralentissement de la vitesse de rafraîchissement de l'écran pour 
    // pour qu'elle soit conforme au clignotement du texte
    arduboy.setFrameRate(1);

    // Si le meilleur score est battu
    if (snake.foodEaten > highScore) {

      // Sauvegarde du meilleur score sur l'EEPROM
      highScore = snake.foodEaten;
      //EEPROM.update(EEPROM_ADDRESS, snake.foodEaten);
    }
  
    // Si le joueur appui sur le bouton B
    if (arduboy.pressed(B_BUTTON)) {

      // Affichage du menu principal
      gameState = GAMESTATE_MAINMENU;
      snake.init(&arduboy, GAMESTATE_MAINMENU);
  
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);

      return;
    }

    // Maintient de la bonne fréquence d'affichage
    if (!arduboy.nextFrame()) return;
    
    arduboy.clear();

    // Affichage du titre "GAME OVER"
    arduboy.setTextSize(2);
    arduboy.setCursor(10, 2);
    arduboy.print("GAME OVER");

    // Affichage du score
    arduboy.setTextSize(1);
    arduboy.setCursor(19, 25);
    arduboy.print("Your score: ");
    arduboy.setCursor(91, 25);;
    sprintf(scoreText, "%03u", snake.foodEaten);
    arduboy.print(scoreText);

    // Affichage du meilleur score
    arduboy.setCursor(19, 40);
		arduboy.print("High score:");
    arduboy.setCursor(91, 40);
    sprintf(highScoreText, "%03u", highScore);
    arduboy.print(highScoreText);
  
    // Affichage d'un message "Press B to continue" clignotant
    blinkingTextState = (blinkingTextState + 1) % 6;
    if (blinkingTextState < 3) {
  
      arduboy.setCursor(7, 55);
      arduboy.print("Press B to continue");
    }
       
    arduboy.display();
  }
  
  // Si le serpent est en vie, la partie continue
  else if (gameState == GAMESTATE_GAME) {

    // Lecture des touches
    if (arduboy.pressed(UP_BUTTON)) {
  
      snake.direction = SNAKEDIR_UP;
    }
    else if (arduboy.pressed(DOWN_BUTTON)) {
      
      snake.direction = SNAKEDIR_DOWN;    
    }
    else if (arduboy.pressed(LEFT_BUTTON)) {
      
      snake.direction = SNAKEDIR_LEFT;    
    }
    else if (arduboy.pressed(RIGHT_BUTTON)) {
      
      snake.direction = SNAKEDIR_RIGHT;    
    }
    else if (arduboy.pressed(A_BUTTON)) {
  
      // Activation ou désactivation du son
      arduboy.setTextSize(1);
      if (snake.canMakeSound) {
  
        snake.canMakeSound = false;
        arduboy.setCursor(28, 0);
        arduboy.print("            ");
        arduboy.setCursor(34, 0);
        arduboy.print("Sound: OFF");
      }
      else {
  
        snake.canMakeSound = true;
        arduboy.setCursor(31, 0);
        arduboy.print("           ");
        arduboy.setCursor(37, 0);
        arduboy.print("Sound: ON");
//        arduboy.tunes.tone(1000, 200);
      }
      arduboy.display();
      delay(1000);
    }
    else if (arduboy.pressed(B_BUTTON)) {
      
      gameState = GAMESTATE_PAUSED;
      
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);    
    }
    
    // Maintient de la bonne fréquence d'affichage
    if (!arduboy.nextFrame()) return;
    
    snake.move();
    if (snake.isDead) return;

    arduboy.clear();

    // Affichage d'une bordure
    arduboy.drawRect(2, 2, 124, 60, 1);

    // Affichage du score
    arduboy.setTextSize(1);
    arduboy.setCursor(49, 0);
    arduboy.print("     ");
    arduboy.setCursor(55, 0);
    sprintf(scoreText, "%03u", snake.foodEaten);
    arduboy.print(scoreText);
   
    snake.draw();
    snake.food.draw();
  
    arduboy.display();
  }

  // Si le jeu est en pause, un message "PAUSED" est affiché
  else if (gameState == GAMESTATE_PAUSED) {

    // Si le joueur appui sur le bouton B la partie reprend
    if (arduboy.pressed(B_BUTTON)) {
    
      gameState = GAMESTATE_GAME;
  
      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);
    }

    // Maintient de la bonne fréquence d'affichage
    if (!arduboy.nextFrame()) return;
    
    // Affichage du titre "PAUSED"
    arduboy.fillRect(22, 18, 84, 28, 0);
    arduboy.drawRect(23, 19, 82, 26, 1);
    arduboy.setTextSize(2);
    arduboy.setCursor(29, 25);
    arduboy.print("PAUSED");
  
    arduboy.display();
  }

  // Par défaut le menu principal est affiché
  else {

    // Modification de la vitesse de rafraîchissement de l'écran pour pour qu'elle 
    // soit conforme au clignotement du texte et au déplacement du serpent
    arduboy.setFrameRate(20);

    // Si le joueur appui sur le bouton B
    if (arduboy.pressed(B_BUTTON)) {

      // Lancement de la partie
      gameState = GAMESTATE_GAME;
      snake.init(&arduboy, GAMESTATE_GAME);

      // Ajout d'un délai pour éviter que la pression de la touche B se répercute 
      // sur l'écran suivant
      delay(200);

      return;
    }
    
    // Maintient de la bonne fréquence d'affichage
    if (!arduboy.nextFrame()) return;

    snake.moveOnMainMenu();
  
    arduboy.clear();
    
    // Affichage du titre
    arduboy.setTextSize(2);
    arduboy.setCursor(10, 11);
    arduboy.print("ARDUSNAKE");
    arduboy.setTextSize(1);
    
    // Affichage d'un message "Press B to start" clignotant
    blinkingTextState = (blinkingTextState + 1) % 30;
    if (blinkingTextState < 15) {
    
      arduboy.setCursor(16, 44);
      arduboy.print("Press B to start");
    }

    snake.draw();
  
    arduboy.display();
  }
}

