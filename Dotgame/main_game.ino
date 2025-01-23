// LiquidCrystal library for the LCD-Display
//  and LedControl library for the MAX7219 Module (led matrix)
#include <LiquidCrystal.h>
#include <LedControl.h>

// built in push-button for the joystick module
const int SW_pin = 2; 
const int buzzer = 3; 

// object for the lcd
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); 
LedControl lc = LedControl(6, 4, 5, 1); 

// digits byte representation for the 1 digit 7-segment display
byte seven_seg_digits[10] = { 
  B11111100,  
  B01100000,  
  B11011010,  
  B11110010,  
  B01100110,  
  B10110110,  
  B10111110,  
  B11100000,  
  B11111110,  
  B11100110   
};

// zero byte
byte solution_byte = B00000000;

// colum and rows for the game
int random_column = 0;
int random_row = 0;
int userStartingColumn = random(0, 8);
int userStartingRow = random(0, 8);
int currentColumn = userStartingColumn;
int currentRow = userStartingRow;

// integers for the game
int turn_firstLevel = 0;
int totalPoints = 0;

void start(){
  lcd.setCursor(0, 0);
  lcd.print("Welcome to this");
  lcd.setCursor(0, 1);
  lcd.print(" shitty game");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("where u suck or");
  lcd.setCursor(0, 1);
  lcd.print("not, we'll see");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("let's start");
  lcd.setCursor(0, 1);
  lcd.print("easy, Level 1");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("a dot blinks");
  lcd.setCursor(0, 1);
  lcd.print(" go to it");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("if u on the dot");
  lcd.setCursor(0, 1);
  lcd.print(" 2 points");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("if near the dot");
  lcd.setCursor(0, 1);
  lcd.print(" 1 points");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("if away from dot");
  lcd.setCursor(0, 1);
  lcd.print(" u suck");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("show me");
  lcd.setCursor(0, 1);
  lcd.print(" what u got");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("each turn u");
  lcd.setCursor(0, 1);
  lcd.print(" have less time");
  buzzerSound(400);
  delay(2200);
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("Level 1: ");
  buzzerSound(400);
  delay(2200);
  lcd.clear();

}

void setup() {
  pinMode(SW_pin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A3, OUTPUT);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  start();
}

void sevenSegWrite(byte digit) {
  digitalWrite(A4, LOW);
  shiftOut(A3, A5, LSBFIRST, seven_seg_digits[digit]);  
  digitalWrite(A4, HIGH);
}

void randomAnswerGenerator() {
  random_column = random(0, 8);
  random_row = random(0, 8);
  
  lc.setRow(0, random_row, solution_byte|(byte)(1 << random_column));
  delay(500);
  lc.clearDisplay(0);
}

void UserControl(int* userStartingColumn, int* userStartingRow, int x_value, int y_value) {
  if ((x_value > 650) && (*userStartingRow < 7)) {
      (*userStartingRow)++;
  } else if ((x_value < 400) && (*userStartingRow > 0)) {
      (*userStartingRow)--;
  }

  if ((y_value > 650) && (*userStartingColumn > 0)) {
      (*userStartingColumn)--;
  } else if ((y_value < 400) && (*userStartingColumn < 7)) {
      (*userStartingColumn)++;
  }

  delay(100);

  if ((userStartingColumn != currentColumn) || (userStartingRow != currentRow)) {
      currentColumn = *userStartingColumn;
      currentRow = *userStartingRow;
      lc.clearDisplay(0);
  }

  lc.setRow(0, currentRow, 1 << currentColumn);
}

void buzzerSound(int delayTime){
  digitalWrite(buzzer, HIGH);
  delay(delayTime);
  digitalWrite(buzzer, LOW);
  delay(delayTime);
}

void firstLevel(int* totalPoints, int turn_firstLevel){

  int digit = 200 - turn_firstLevel*20;
  bool buttonPushed = false;


  randomAnswerGenerator();
  lc.setRow(0, userStartingRow, 1 << userStartingColumn);

  while (digit > 0) {
      int buttonState = digitalRead(SW_pin);
      int y_value = analogRead(A0);
      int x_value = analogRead(A1);

      if (buttonState == 0) {
          if ((random_column == currentColumn) && (random_row == currentRow)) {
              buttonPushed = true;
              lcd.setCursor(0, 0);
              lcd.print("u not bad");
              lcd.setCursor(0, 1);
              lcd.print("2 points to u");
              delay(2200);
              lcd.clear();
              (*totalPoints) += 2;
          } else if (((random_column-1 <= currentColumn) && (currentColumn <= random_column+1)) && ((random_row-1 <= currentRow) && (currentRow <= random_row+1))){
              buttonPushed = true;
              lcd.setCursor(0, 0);
              lcd.print("mehh it works");
              lcd.setCursor(0, 1);
              lcd.print("1 points to u");
              delay(2200);
              lcd.clear();
              (*totalPoints)++;
          } else {
            lcd.setCursor(0, 0);
            lcd.print("u suck");
            lcd.setCursor(0, 1);
            lcd.print("no points to u");
            delay(2200);
            lcd.clear();
          }

          currentRow = random(0, 8);
          currentColumn = random(0, 8);

          break;
      }

      UserControl(&userStartingColumn, &userStartingRow, x_value, y_value);
      delay(25);
      if (digit%20 == 0){
        sevenSegWrite(digit/20-1);
      }

      digit--;

  }


  delay(1000);  
}

void loop() {

  for (int turn = 0; turn < 5; turn++){
    firstLevel(&totalPoints, turn);
  }
  lcd.setCursor(0, 0);
  lcd.print("your point is: ");
  lcd.setCursor(0, 1);
  lcd.print(totalPoints);

  exit;
  
}
