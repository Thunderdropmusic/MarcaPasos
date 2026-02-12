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
  if (not(pot1value == potSaveValue)){
    timeShowPotValue = millis();
    screenPotes = true;
    menuPrincipal = false; 
    potSaveValue = pot1value;
  }
  if (screenPotes == true){
    option1 == false;
    option3 == false;
    screen_Potes();
  }
  else if(option1 == true){
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
  lcd.print(pot1value);
  lcd.setCursor(0,1);
  lcd.print(millis());
  lcd.print(" ");
  lcd.print(timeShowPotValue);
  if(millis() - timeShowPotValue >= 1000){
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
    lcd.setCursor(0,1);
    lcd.print(pot1value);
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
