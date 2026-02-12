#include <LiquidCrystal.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int pinButton1 = 22;
const int pinButton2 = 24;
const int pinButton3 = 26;
const int pinButton4 = 28;
int potesPin[2] = {A0, A1};

int button1;
int button2;
int button3;
int button4;

const int numPotes = 2;
int potes[2];
int potesValue[2];
int movedPoteValue;



int potSaveValue [2];
long int timeShowPotValue;
long int timeClearScreenValue = 0;
int tiempoActual;
boolean option1 = true;
boolean option2 = false;
boolean option3 = false;
boolean menuPrincipal = true;
boolean menu1 = false;
boolean menu2 = false;
boolean screenPotes = false;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT);
  for(int i = 0;i<numPotes;i++){
    potes[i] = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(potes[i],1018,8,0,127),0,127);
    potSaveValue[i] = potesValue[i];
  }
}

void loop() {
  tiempoActual = millis();
  // put your main code here, to run repeatedly:
  button1 = digitalRead(pinButton1);
  button2 = digitalRead(pinButton2);
  button3 = digitalRead(pinButton3);
  button4 = digitalRead(pinButton4);
  for(int i = 0;i<numPotes;i++){
    potes [i] = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(potes [i],1018,8,0,127),0,127);
  }

  if (menuPrincipal == true){
    menu_Principal();
  }
  else if (screenPotes == true){
    screen_Potes();
  }
  else if (menu1 == true){
    menu_1();
  }
  else if (menu2 == true){
    menu_2();
  }
  refreshScreen();
}


void refreshScreen(){
  if (tiempoActual - timeClearScreenValue > 100){
    lcd.clear();
    timeClearScreenValue = millis();
  }
}
