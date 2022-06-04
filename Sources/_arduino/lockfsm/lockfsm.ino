#include <LiquidCrystal.h>
#include <Keypad.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal LCD(rs, en, d4, d5, d6, d7);
char keys[4][4] = {
    {'1','2','3', 'A'},
    {'4','5','6', 'B'},
    {'7','8','9', 'C'},
    {'*','0','#', 'D'}
  };
byte rowPins[4] = {8, 9, 10, 11};
byte colPins[4] = {A3, A2, A1, A0};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
enum States {Lock, CheckPass, WrongPass, Unlock, UnlockMenu, ToLock, ChangePass, NightMode} state;
char passGuess[] = {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E'};
char passCode[] = {'1', '2', '3', '4', '5', '6', 'E', 'E'};
char userInput = ' ';
int lockCount = 0;
int changeCount = 0;
bool lockInitFlag = false;
bool checkFlag = true;
bool unlockFlag = false;
bool nightFlag = false;
bool changeFlag = false;
int unlockCount = 0;
int adcVal = 0;
  
void setup() { 
  LCD.begin(16, 2);
  LCD.clear();
  LCD.setCursor(0, 0);
  state = Lock;
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  switch (state) {
    case Lock:
      if (!lockInitFlag) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("Enter code:");
        LCD.setCursor(0, 1);
        lockInitFlag = true;
        lockCount = 0;
        for (int i = 0; i < 8; ++i) {
          passGuess[i] = 'E';
        }

        digitalWrite(13, LOW);
        
      }
      
      userInput = keypad.getKey();
      if (userInput) {
        if ((userInput == 'A') || (userInput == 'B') || (userInput == 'C') || (userInput == 'D') || (userInput == '*')) {
          break;
        }
        
        if (userInput == '#') {
          state = CheckPass;
          lockInitFlag = false;
          lockCount = 0;
        }
        else {
          passGuess[lockCount] = userInput;
          LCD.write(userInput);
          ++lockCount;
          LCD.setCursor(lockCount, 1);
          if (lockCount == 8) {
            state = CheckPass;
            lockInitFlag = false;
            lockCount = 0;
          }
        }
      }
      break;
    case CheckPass:
      checkFlag = true;
      for (int i = 0; i < 8; ++i) {
        if (passGuess[i] != passCode[i]) {
          state = WrongPass;
          checkFlag = false;
        }
      }
      if (checkFlag) {
        state = Unlock;
      }
      break;
    case WrongPass:
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("Wrong code!");
      delay(1500);
      state = Lock;
      break;
    case Unlock:
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("Unlocking...");

      digitalWrite(13, HIGH);
      
      delay(1500);
      state = UnlockMenu;
      break;
    case UnlockMenu:
      if (!unlockFlag) {
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("* - Change code");
        LCD.setCursor(0, 1);
        LCD.print("# - Night  mode");
        unlockFlag = true;
      }

      userInput = keypad.getKey();
      if (userInput) {
        if (userInput == '*') {
          state = ChangePass;
          unlockFlag = false;
          unlockCount = 0;
          break;
        }
        else if (userInput == '#') {
          state = NightMode;
          unlockFlag = false;
          unlockCount = 0;
          break;
        }
      }
      
      delay(1);
      ++unlockCount;
      if (unlockCount >= 7000) {
        state = ToLock;
        unlockFlag = false;
        unlockCount = 0;
      }
      break;
    case ToLock:
      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("Locking...");

      digitalWrite(13, LOW);
      
      delay(1500);
      state = Lock;
      break;
    case ChangePass:
      if (!changeFlag) {
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("Enter new code:");
        LCD.setCursor(0, 1);
        changeFlag = true;
        for (int i = 0; i < 8; ++i) {
          passCode[i] = 'E';
        }
      }
      
      userInput = keypad.getKey();
      if (userInput) {
        if ((userInput == 'A') || (userInput == 'B') || (userInput == 'C') || (userInput == 'D') || (userInput == '*')) {
          break;
        }
        
        if (userInput == '#') {
          state = ToLock;
          changeFlag = false;
          changeCount = 0;
        }
        else {
          passCode[changeCount] = userInput;
          LCD.write(userInput);
          ++changeCount;
          LCD.setCursor(changeCount, 1);
          if (changeCount == 6) {
            state = ToLock;
            changeFlag = false;
            changeCount = 0;
          }
        }
      }
      break;
    case NightMode:
      if (!nightFlag) {
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("Night mode:");
        LCD.setCursor(0, 1);
        LCD.print("Press * to leave");
        nightFlag = true;
      }

      adcVal = analogRead(A5);
      if (adcVal < 100) {
        LCD.setCursor (12, 0);
        LCD.print(" L");
        digitalWrite(13, LOW);
      }
      else {
        LCD.setCursor (12, 0);
        LCD.print("UL");
        digitalWrite(13, HIGH);
      }

      userInput = keypad.getKey();
      if (userInput == '*') {
        state = ToLock;
        nightFlag = false;
        break;
      }
      break;
    default:
      state = Lock;
      break;
  }
}
