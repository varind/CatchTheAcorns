/*
   Catch the Acorns by Ben Lipsey www.varind.com 2016. This code is public domain, enjoy!
   https://github.com/varind/CatchTheAcorns
   
   Uses an Arduino, a 16x2 LCD, 3 buttons, 5 NeoPixels, and a piezo 
*/
#include <EEPROM.h>
#include "pitches.h"
#include <LiquidCrystal.h>
LiquidCrystal lcd(4,5,6,7,8,9);

#define POTPIN A0
#define BUTTON01 A3
#define BUTTON02 A2
#define BUTTON03 A1
#define SPEAKER 5
#define LEDPIN 12
#define LEDCOUNT 5
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT, LEDPIN, NEO_GRB + NEO_KHZ800);

long timer = 0;
byte x, lastX, basket, basketX, randNumber, randomCheck;
int y;
bool gameInit = false, cleared = false, createBasket = false;
byte score = 0, hScore = 0, miss = 0;
bool quiet = false;
int startSpeed, speedStep, speedStep2;


byte basketL[8] =
{
  B01110,
  B11100,
  B11100,
  B11100,
  B11100,
  B11110,
  B11111,
  B01111
};
byte basketM[8] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B11111
};
byte basketR[8] =
{
  B01110,
  B00111,
  B00111,
  B00111,
  B00111,
  B01111,
  B11111,
  B11110
};
byte apple[8] =
{
  B00100,
  B01110,
  B01010,
  B11111,
  B10101,
  B01010,
  B00100,
  B00000
};
byte appleB[8] =
{
  B00100,
  B01110,
  B01010,
  B11111,
  B10101,
  B01010,
  B10101,
  B11111
};

void setup() {
  Serial.begin(9600);
  lcd.begin(20, 4);
  pinMode(BUTTON01, INPUT);
  digitalWrite(BUTTON01, HIGH);
  pinMode(BUTTON02, INPUT);
  digitalWrite(BUTTON02, HIGH);
  pinMode(BUTTON03, INPUT);
  digitalWrite(BUTTON03, HIGH);
  randomSeed(analogRead(4));
  strip.begin();
  for (int q = 0; q < LEDCOUNT; q++) {
    strip.setPixelColor(q, 0, 0, 0);
  }
  strip.show();
  lcd.createChar(1, basketL);
  lcd.createChar(2, basketM);
  lcd.createChar(3, basketR);
  lcd.createChar(4, apple);
  lcd.createChar(5, appleB);
}


void loop() {
  game();
}


void game() {
  if (!gameInit) {
    newGame();
  }
  getInput();
  drawBaskets();
  acornDrop();
}

void newGame() {
  for (int q = 0; q < LEDCOUNT; q++) {
    strip.setPixelColor(q, 0, 0, 0);
  }
  strip.show();
  lcd.clear();
  newX();
  y = -1; basket = 1; basketX = 9; score = 0; miss = 0; startSpeed = 1000; speedStep = 10; speedStep2 = 2;
  gameInit = true;
  lcd.setCursor(2, 1);
  lcd.print("CATCH THE ACORNS");
  lcd.setCursor(2, 2);
  lcd.print("  (ACORN HERO)");
  if (digitalRead(BUTTON01) == LOW && digitalRead(BUTTON02) == LOW && digitalRead(BUTTON03) == LOW) {
    EEPROM.put(0, 0);
    lcd.setCursor(2, 3);
    lcd.print("HIGH SCORE RESET");
  }
  playIntro();
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 4; j++) {
      lcd.setCursor(i, j);
      lcd.write(4);
      delay(30);
    }
  }
  lcd.clear();
  delay(100);
  lcd.setCursor(4, 1);
  lcd.print("READY ");
  for (int q = 0; q < LEDCOUNT; q++) {
    strip.setPixelColor(q, 0, 10, 0);
  }
  strip.show();
  if (!quiet) tone(SPEAKER, NOTE_B4, 500);
  delay(1000);
  lcd.print("SET ");
  for (int q = 0; q < LEDCOUNT; q++) {
    strip.setPixelColor(q, 15, 10, 0);
  }
  strip.show();
  if (!quiet) tone(SPEAKER, NOTE_B4, 500);
  delay(1000);
  lcd.print("GO!");

  for (int q = 0; q < LEDCOUNT; q++) {
    strip.setPixelColor(q, 10, 0, 0);
  }
  strip.show();
  if (!quiet) tone(SPEAKER, NOTE_FS5, 500);
  delay(1000);
  lcd.clear();
}

void getInput() {
  if (digitalRead(BUTTON01) == LOW) {
    basket = 0;
    basketX = 2;
    cleared = false;
    createBasket = false;
    playButton();
  }
  if (digitalRead(BUTTON02) == LOW) {
    basket = 1;
    basketX = 9;
    cleared = false;
    createBasket = false;
    playButton();
  }
  if (digitalRead(BUTTON03) == LOW) {
    basket = 2;
    basketX = 17;
    cleared = false;
    createBasket = false;
    playButton();
  }
}

void drawBaskets() {
  if (basket == 0) {
    clearBaskets();
    if (!createBasket) {
      lcd.setCursor(1, 3);
      lcd.write(1); lcd.write(2); lcd.write(3);
      createBasket = true;
    }
  }
  if (basket == 1) {
    clearBaskets();
    if (!createBasket) {
      lcd.setCursor(8, 3);
      lcd.write(1); lcd.write(2); lcd.write(3);
      createBasket = true;
    }
  }
  if (basket == 2) {
    clearBaskets();
    if (!createBasket) {
      lcd.setCursor(16, 3);
      lcd.write(1); lcd.write(2); lcd.write(3);
      createBasket = true;
    }
  }
}

void acornDrop(){

  if (timer + startSpeed < millis()) {
    startSpeed = startSpeed - speedStep;
    if (startSpeed < 100) startSpeed = 100;
    if (startSpeed < 500) speedStep = speedStep2;
    if (y == 1 || y == 2 || y == 3) {
      lcd.setCursor(x, y - 1);
      lcd.write(254);
    }
    if (y == -1 || y == 0) {
      lcd.setCursor(x, 3);
      lcd.write(254);
      cleared = false;
      clearBaskets();
      createBasket = false;
    }
    if (y >= 0) {
      lcd.setCursor(x, y);
      lcd.write(4);
    }
    if (x != basketX && y == 3) {
      miss++;
      for (int q = 0; q < miss; q++) {
        strip.setPixelColor(4 - q, 0, 10, 0);
      }
      strip.show();

      lcd.setCursor(5, 0);
      lcd.print("YOU MISSED!");
      lcd.setCursor(2, 1);
      lcd.print("YOUR SCORE IS: ");
      lcd.print(score);
      playMiss();
      if (miss >= 5) {
        highScore();
        lcd.clear();
        lcd.setCursor(3, 1);
        lcd.print("- GAME OVER -");
        lcd.setCursor(3, 2);
        lcd.print("FINAL SCORE: ");
        lcd.print(score);
        lcd.setCursor(3, 3);
        lcd.print(" HIGH SCORE: ");
        lcd.print(hScore);
        playLost();
        gameInit = false;
      }
      delay(1000);
    }

    if (x == basketX && y == 3) {
      score++;
      lcd.setCursor(x, y);
      lcd.write(5);
      lcd.setCursor(5, 0);
      lcd.print("GOOD CATCH!");
      lcd.setCursor(2, 1);
      lcd.print("YOUR SCORE IS: ");
      lcd.print(score);
      playCatch();
      delay(1000);
    }

    if (y == 3) {
      newX();
      lcd.setCursor(0, 0);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print("                    ");
    }
    if (randomCheck >= 2)newX();
    y++;
    if (y > 3) y = -1;
    timer = millis();
  }  
}

void newX() {
  randNumber = random(3);
  lastX = x;
  if (randNumber == 0) x = 2;
  if (randNumber == 1) x = 9;
  if (randNumber == 2) x = 17;
  if (lastX == x) {
    randomCheck++;
  } else {
    randomCheck = 0;
  }
}

void highScore() {
  EEPROM.get(0, hScore);
  if (hScore < score) {
    EEPROM.update(0, score);
    hScore = score;
  }
}

void clearBaskets() {
  if (!cleared) {
    lcd.setCursor(1, 3);
    lcd.write(254); lcd.write(254); lcd.write(254);
    lcd.setCursor(8, 3);
    lcd.write(254); lcd.write(254); lcd.write(254);
    lcd.setCursor(16, 3);
    lcd.write(254); lcd.write(254); lcd.write(254);
    cleared = true;
  }
}

void playButton() {
  if (!quiet)  tone(SPEAKER, NOTE_C6, 20);
}

void playCatch() {
  if (!quiet) tone(SPEAKER, NOTE_C4, 200);
  delay(200);
  if (!quiet) tone(SPEAKER, NOTE_C5, 200);
  delay(200);
  if (!quiet) tone(SPEAKER, NOTE_C6, 200);
  delay(1500);
}

void playMiss() {
  if (!quiet) tone(SPEAKER, NOTE_CS5, 200);
  delay(200);
  if (!quiet) tone(SPEAKER, NOTE_C5, 200);
  delay(1500);
}

void playIntro() {
  if (!quiet) tone(SPEAKER, NOTE_B4, 175);
  delay(200);
  if (!quiet) tone(SPEAKER, NOTE_B4, 75);
  delay(100);
  if (!quiet) tone(SPEAKER, NOTE_B4, 75);
  delay(100);
  if (!quiet) tone(SPEAKER, NOTE_FS5, 500);
  delay(1500);
}

void playLost() {
  if (!quiet) tone(SPEAKER, NOTE_DS5, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_CS5, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_B4, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_A4, 500);
  delay(500);
  if (!quiet) tone(SPEAKER, NOTE_C5, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_AS4, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_A4, 150);
  delay(150);
  if (!quiet) tone(SPEAKER, NOTE_G4, 500);
  delay(3000);
}
