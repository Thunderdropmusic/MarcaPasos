void menu_Principal(){
  if(button1 == HIGH){
    option1 = true;
    option3 = false;
  }

  if((button2 == HIGH) && (option1 == true)){
    menu1 = true;
    menuPrincipal = false;
  }
  else if((button2 == HIGH) & (option3 == true)){
    menu2 = true;
    menuPrincipal = false;
  }

  if(button3 == HIGH){
    option1 = false;
    option3 = true;
  }
  for(int i = 0;i<numPotes;i++){
    if (not(potesValue[i] == potSaveValue [i])){
      timeShowPotValue = tiempoActual;
      screenPotes = true;
      menuPrincipal = false;
      potSaveValue[i] = potesValue[i];
      movedPoteValue = potesValue[i];
      break;
    }
  }
  if (screenPotes == true){
    option1 == false;
    option3 == false;
    screen_Potes();
  }
  if(option1 == true){
    lcd.print("0 menu 1");
    lcd.setCursor(0,1);
    lcd.print("  menu 2");
  }
  else if(option3 == true){
    lcd.print("  menu 1");
    lcd.setCursor(0,1);
    lcd.print("0 menu 2");    
  }
}
void screen_Potes(){
  lcd.print(movedPoteValue);
  if(tiempoActual - timeShowPotValue >= 1000){
    screenPotes = false;
    menuPrincipal = true;
  }
  if((button2 == HIGH) && (option1 == true)){
    menu1 = true;
    menuPrincipal = false;
    screenPotes = false;
  }
  else if((button2 == HIGH) & (option3 == true)){
    menu2 = true;
    menuPrincipal = false; 
    screenPotes = false; 
  }
}
void menu_1(){
  lcd.print("menu 1");
  if(button4 == HIGH){
    menuPrincipal = true;
    menu1 = false;
  }
}

void menu_2(){
  lcd.print("menu 2");
  if(button4 == HIGH){
    menuPrincipal = true;
    menu2 = false;
  }
}
