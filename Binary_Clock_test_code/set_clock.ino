bool set_button_pressed(){
  DDRA |= (0X01 << SET_PIN);
  //PORTA |= (0 << led_pins[0]);  //It should already be 0
  //DDRA |= (0x00
  //PORTA |= (0x01 << BUTTONS); //Set PullUp Resistance in pin num 7 PORT A
  PORTB |=(0x01 << 2);    //PORT B PIN 2
  __asm__ __volatile__ ("nop\n\t");
  //bool state = !((PINA >> BUTTONS)&0x01);
  bool state = !((PINB >> 2)&0x01);
  /*PORTA= 0x00;
  DDRA = 0x00;*/
  PORTA= PORTA & B01010000;//0x00;
  DDRA = DDRA & B01010000;//0x00;
  return state;
}
bool select_button_pressed(){
  DDRA |= (0X01 << SELECT_PIN);
  //PORTA |= (0 << led_pins[0]);  //It should already be 0
  //DDRA |= (0x00
  PORTA |= (0x01 << BUTTONS); //Set PullUp Resistance in pin num 7 PORT A
  __asm__ __volatile__ ("nop\n\t");
  //__asm__("nop\n\t");
  volatile bool state = !((PINA >> BUTTONS)&0x01);//!digitalRead(BUTTONS);//!((PINA >> BUTTONS)&0x01);   //This seems to be provoking an undesired blink on the clock leds
  /*PORTA= 0x00;
  DDRA = 0x00;*/
  PORTA= PORTA & B01010000;//0x00;
  DDRA = DDRA & B01010000;//0x00;
  return state;
}
void selfsetHour(){
  if(set_button_pressed()){
    set_button_counter++;
    unsigned long start=millis();
    bool par;
//    while(set_button_pressed()&&millis()<(start+TIMESET_TIMEOUT*10)){
//      printHoras(set_button_counter, Agrupacion_M, true, false);
//    }
    if(set_button_counter==5){
          while((set_button_counter==5)&&((millis()-start)<(TIMESET_TIMEOUT*500))){
      for(long t=millis(); (millis()-t)<600;){
        //printHoras(change_pos+1, Agrupacion_M, true, false);
        //printHoras(0x22, Agrupacion_S, false, false);
        switch (change_pos){
          case 0:
            printHoras(time.hour, Horas_pins, false, true);
            if(par){
              printHoras(time.minute, Agrupacion_M, true, false);
              printHoras(time.second, Agrupacion_S, false, false);
            }
            break;
          case 1:
            printHoras(time.minute, Agrupacion_M, true, false);
            if(par){
              printHoras(time.hour, Horas_pins, false, true);
              printHoras(time.second, Agrupacion_S, false, false);
            }
            break;
          case 2:
            printHoras(time.second, Agrupacion_S, false, false);
            if(par){
              printHoras(time.hour, Horas_pins, false, true);
              printHoras(time.minute, Agrupacion_M, true, false);
            }
            break;
        }
      }
      par=!par;
      if(set_button_pressed()){
        change_pos++;
        start=millis();
        if(change_pos==3) change_pos=0;
      }
      pinMode(PA7, INPUT_PULLUP);
      if(!digitalRead(PA7)){ //if(select_button_pressed()){ //NO se... hay algo que hace que lo marque como presionado solo en esta funcion...
        switch (change_pos){
          case 0:
            time.hour++;
          if(!((time.hour&0x0F)^0x0A)){
            time.hour=(time.hour&0x30)+0x10;
          }
          else if(time.hour==0x24){
            time.hour=0;
          }
            break;
          case 1:
            time.minute++;
            if(!((time.minute&0x0F)^0x0A)){
              time.minute=(time.minute&0xF0)+0x10;
              if(!((time.minute&0xF0)^0x60)){
                time.minute=0;
              }
            }
          break;
          case 2:
            time.second++;
            if(!((time.second&0x0F)^0x0A)){
              time.second=(time.second&0xF0)+0x10;
              if(!((time.second&0xF0)^0x60)){
                time.second=0;
              }
            }
            break;
        }
        start=millis();
      }
      PORTA= PORTA & B01010000;//0x00;
      DDRA = DDRA & B01010000;//0x00;
      
    }
    DS3231_set_time(&time);
    }

  }
  else{
    set_button_counter=0;
  }
  
}
