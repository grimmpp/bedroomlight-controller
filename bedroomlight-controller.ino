/**
 * 
 * Light Controller with one spincontrol and one button.
 * 
 * How to use:
 * 
 * Push button to switch on.
 * Turn spincontrol to adjust brightness.
 * 
 * Push button shortly in order to go into color menu. \
 * First color will blink and then you can adjust the color brigtness by truning the spincontrol. Push again and do the same for the next two colors, and one more push to save. There is a timeout of 10s after it, it will leave the menu automatically.
 * 
 * Hold the button for 1s in order to switch the light off.
 */


#include <EEPROM.h>

#define led_r_pin 11
#define led_g_pin 10
#define led_b_pin 9

#define taster_pin 12

#define poti_pin A0

#define MenuOff 0
#define MenuChangeRedInit 1
#define MenuChangeRed 2
#define MenuChangeGreenInit 3
#define MenuChangeGreen 4
#define MenuChangeBlueInit 5
#define MenuChangeBlue 6
#define MenuSave 7
#define SwitchOff 10

#define flashVal 100
#define loopDelay 50
#define menuTimeout 10000 //10 sec
#define switchOffTimeout 1000

bool tasterPressed = false;
int tasterStatus = 0;
int timer = 0;

int ledValRed = 100;
int ledValGreen = 100;
int ledValBlue = 100;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  pinMode(led_r_pin, OUTPUT);
  pinMode(led_g_pin, OUTPUT);
  pinMode(led_b_pin, OUTPUT);

  analogWrite(led_r_pin, 0);
  analogWrite(led_g_pin, 0);
  analogWrite(led_b_pin, 0);

  pinMode(taster_pin, INPUT_PULLUP);

  pinMode(poti_pin, INPUT);

  readColorValues();
}

int potiVal = 0;
int menuMode = MenuOff;

void loop() {

  // read taster
  tasterStatus = digitalRead(taster_pin);
  if (digitalRead(taster_pin) == LOW) { 
    tasterPressed=true;
  }

  //read poti
  potiVal = analogRead(poti_pin);

  if (tasterPressed && tasterStatus == HIGH) {
    tasterPressed = false;
    if ((menuMode == MenuOff || menuMode == SwitchOff) && timer >= switchOffTimeout) {
      menuMode = SwitchOff;
    } else {
      menuMode++;
    }
    timer = 0;
  } else if (tasterPressed && tasterStatus == LOW) {
    timer += loopDelay;
    if (menuMode == MenuOff && timer >= switchOffTimeout) {
      menuMode = SwitchOff;
    }
  }

  if (timer > menuTimeout) {
    menuMode = MenuOff;
    timer=0;
  }

  switch(menuMode) {
    case MenuChangeRedInit:
      flashLed(flashVal,0,0);
      menuMode++;
      timer += loopDelay;
    break;

    case MenuChangeRed:
      ledValRed = potiVal /4;
      setLedColor(ledValRed,0,0);
      timer += loopDelay;
    break;
    
    case MenuChangeGreenInit:
      flashLed(0,flashVal,0);
      menuMode++;
      timer += loopDelay;
    break;

    case MenuChangeGreen:
      ledValGreen = potiVal /4;
      setLedColor(0, ledValGreen ,0);
      timer += loopDelay;
    break;
    
    case MenuChangeBlueInit:
      flashLed(0,0,flashVal);
      menuMode++;
      timer += loopDelay;
    break;

    case MenuChangeBlue:
      ledValBlue = analogRead(poti_pin) /4;
      setLedColor(0,0,ledValBlue);
      timer += loopDelay;
    break;

    
      saveColorValues();
      menuMode = MenuOff;
      timer += loopDelay;
    break;

    case SwitchOff:
      ledOff();
    break;
    
    default:
      menuMode = MenuOff;
    
      int multiVal = potiVal/10;
      const int diffVal = 25;

      int redVal = min( (ledValRed*multiVal) /diffVal ,255);
      int greenVal = min( (ledValGreen*multiVal) /diffVal ,255);
      int blueVal = min( (ledValBlue*multiVal) /diffVal ,255);
    
      setLedColor(
        redVal,
        greenVal,
        blueVal);
    break;
  }

  int multiVal = potiVal/10;
  const int diffVal = 25;

  int redVal = min( (ledValRed*multiVal) /diffVal ,255);
  int greenVal = min( (ledValGreen*multiVal) /diffVal ,255);
  int blueVal = min( (ledValBlue*multiVal) /diffVal ,255);

  Serial.print(" MenuMode: "); Serial.println(menuMode);
  Serial.print(" tasterPressed: "); Serial.print(tasterPressed);
  Serial.print(" timer: "); Serial.print(timer);
  Serial.print(" valRed: "); Serial.print(ledValRed);
  Serial.print(" multiVal: "); Serial.print(multiVal);
  Serial.print(" Red: "); Serial.print(redVal);
  Serial.print(" Green: "); Serial.print(greenVal);
  Serial.print(" Blue: "); Serial.print(blueVal);
  Serial.println();

  delay(loopDelay);
}

void ledOff() {
  setLedColor(0,0,0);
}

void flashLed(int r, int g, int b) {
  for(int i=0;i<2;i++) {
    ledOff();
    delay(250);
    setLedColor(r,g,b);
    delay(250);
  }
}

void setLedColor(int r, int g, int b) {
  analogWrite(led_r_pin, r);
  analogWrite(led_g_pin, g);
  analogWrite(led_b_pin, b);
}

void readColorValues() {
  int addr = 0;
  ledValRed = EEPROM.read(addr);
  addr++;
  ledValGreen = EEPROM.read(addr);
  addr++;
  ledValBlue = EEPROM.read(addr);
}

void saveColorValues() {
  int addr = 0;
  EEPROM.write(addr, ledValRed);
  addr++;
  EEPROM.write(addr, ledValGreen);
  addr++;
  EEPROM.write(addr, ledValBlue);
}

