#include <LiquidCrystal.h>

LiquidCrystal lcd(10, 9, 8, 7, 6, 5);

const int EN1 = 13;
const int IN1 = 12;
const int IN2 = 11;
const int LED_DOWN = 4;
const int LED_UP   = 3;

const int sensors[10] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};


const int inRows[3]  = {24, 23, 22};
const int inCols[4]  = {26, 28, 30, 32};
const int outRows[3] = {41, 39, 37};
const int outCols[4] = {43, 45, 47, 49};

char keypadMap[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

int currentFloor = 0;   
int targetFloor  = 0;   
unsigned long lastKeyPressTime = 0; 
const int debounceTime = 200; 

bool insideTaskActive = false; 

void setup() {
  pinMode(EN1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(LED_UP, OUTPUT);
  pinMode(LED_DOWN, OUTPUT);

  for (int i = 0; i < 10; i++) pinMode(sensors[i], INPUT);

  
  for (int i = 0; i < 3; i++) {
    pinMode(inRows[i], INPUT_PULLUP);
    pinMode(outRows[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(inCols[i], OUTPUT); digitalWrite(inCols[i], HIGH);
    pinMode(outCols[i], OUTPUT); digitalWrite(outCols[i], HIGH);
  }

  lcd.begin(16, 2);
  lcd.print("hello");
  delay(1000);
  lcd.clear();
}

void loop() {
  
  for (int i = 0; i < 10; i++) {
    if (digitalRead(sensors[i]) == HIGH) {
      currentFloor = i; 
    }
  }

  if (millis() - lastKeyPressTime > debounceTime) {
    char keyIn  = scanKeypad(inRows, inCols);
    char keyOut = scanKeypad(outRows, outCols);

    if (keyIn != '\0' && keyIn >= '0' && keyIn <= '9') {
        targetFloor = keyIn - '0';
        insideTaskActive = true; 
        lastKeyPressTime = millis();
        
        lcd.setCursor(13, 0);
        lcd.print(" IN"); 
    } 
   
    else if (keyOut != '\0' && keyOut >= '0' && keyOut <= '9') {
      if (insideTaskActive == false) { 
        targetFloor = keyOut - '0';
        lastKeyPressTime = millis();
        
        lcd.setCursor(13, 0);
        lcd.print("OUT");
      }
    }
  }

  
  if (currentFloor < targetFloor) {
    moveUp();
  } 
  else if (currentFloor > targetFloor) {
    moveDown();
  } 
  else {
    stopElevator();
    insideTaskActive = false; 
    lcd.setCursor(13, 0);
    lcd.print("   ");
  }

  updateDisplay();
}

char scanKeypad(const int* rows, const int* cols) {
  for (int col = 0; col < 4; col++) {
    digitalWrite(cols[col], LOW);
    for (int row = 0; row < 3; row++) {
      if (digitalRead(rows[row]) == LOW) {
        digitalWrite(cols[col], HIGH);
        return keypadMap[col][row];
      }
    }
    digitalWrite(cols[col], HIGH);
  }
  return '\0';
}

void moveUp() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  analogWrite(EN1, 255);
  digitalWrite(LED_UP, HIGH); digitalWrite(LED_DOWN, LOW);
}

void moveDown() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  analogWrite(EN1, 255);
  digitalWrite(LED_UP, LOW); digitalWrite(LED_DOWN, HIGH);
}

void stopElevator() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  analogWrite(EN1, 0);
  digitalWrite(LED_UP, LOW); digitalWrite(LED_DOWN, LOW);
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Floor: "); lcd.print(currentFloor);
  lcd.setCursor(0, 1);
  lcd.print("Dest : "); lcd.print(targetFloor);
  lcd.print("    ");
}