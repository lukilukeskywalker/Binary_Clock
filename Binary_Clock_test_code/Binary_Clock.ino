  //LED DRIVERS DEFINITIONS
//int led_pins[]={PINF0, PINF1, PINF4, PINF6};
#include <avr/wdt.h>
//#include <DS3231.h>

#if defined(ARDUINO_AVR_MICRO)
#include <Wire.h>
const int led_pins[]={A5, A4, A3, A2, A1};  // P1, P2, P3, P4, P5
//int Horas_pins[]={A5, A4, A3};
//int Minutos_pins[]={A2, A4, A1};  //Este caso es complejo. La parte superior no se selecciona, con el 1 pin, sino con un cuarto. P1, A5. Ademas selecciona la seccion alta del segundo segmento.
//int Segundos_pins[]={A3, A2, A1};
//int Agrupacion_M[]={A5, A2, A4, A5, A4, A2};
//int Agrupacion_S[]={A5, A2, A3, A3, A2, A1};
#elif defined(__AVR_ATtiny84__)
#include <USIWire.h>
int led_pins[]={0, 1, 2, 3, 5};  // P1, P2, P3, P4, P5 //Attiny84 Positions PORTA0, PORTA1, PORTA2, PORTA3, PORTA5
#define BUTTONS 7 //I think I will look for the button pressed by toggling the led_pins 
#define SET_PIN led_pins[0]
#define SELECT_PIN led_pins[1]
#endif
#define TIMESET_TIMEOUT 60    //60 seconds to make a change.
int Horas_pins[]={led_pins[0], led_pins[1], led_pins[2]};
int Minutos_pins[]={led_pins[0], led_pins[1], led_pins[4]};
int Segundos_pins[]={led_pins[2], led_pins[3], led_pins[4]};
int Agrupacion_H[]={led_pins[0],led_pins[1],led_pins[2],led_pins[0],led_pins[1],led_pins[2]};
int Agrupacion_M[]={led_pins[0],led_pins[3],led_pins[1], led_pins[0], led_pins[1], led_pins[4]};
int Agrupacion_S[]={led_pins[0],led_pins[3],led_pins[2],led_pins[2],led_pins[3],led_pins[4]};

//#include <RtcDS3231.h>
//RtcDS3231<TwoWire> Rtc(Wire);
//DS3231 Clock;
struct time_struct{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
time_struct time;
unsigned long time_elapsed=0;
byte set_button_counter=0;
byte change_pos=0;
void setup() {
  //wdt_disable();
  Wire.begin();
  DS3231_init();
  //Rtc.Begin();
  DS3231_get_time(&time);
}
void setPin(int pin, bool state){
  DDRA |= (0X01 << pin);
  PORTA |= (state << pin);
}
void printHoras(byte numero, int* pines, bool not_minutes, bool horas){
  //El booleano not_minutes se usa en el 3 led del segundo segmento, para determinar que lado de los 2 es (Minutos o segundos)
  byte primer_digito = numero & 0x0F;//numero%10; //Guardamos el primer digito
  byte segundo_digito = (numero>>4)&0x0F;//numero/10;//Guardamos el segundo digito
  int length = 5;//(sizeof(pines)/sizeof(pines[0]))-1;//5;//sizeof(pines);
  if(horas) length=2;
  for(int i=0; i<4; i++){
    if((primer_digito>>i)&0x01){
      pinMode(pines[length], OUTPUT); //Lo pongo en en OUTPUT, porque esta seccion del segmento sera la que manejemos prmero.
      //Estos son los casos para la primera parte del segmento, que esta controlada por la ultima patilla en el array
      ////Serial.println(String("Comprobacion Realizada Bit numero: ")+i+String("Byte: ")+ primer_digito);
      switch(i){
        case 0:
          /*digitalWrite(pines[length], HIGH);
          pinMode(pines[length-1], OUTPUT);
          digitalWrite(pines[length-1], LOW);*/
          setPin(pines[length], 1);
          setPin(pines[length-1], 0);
          break;
        case 1:
          /*digitalWrite(pines[length], LOW);
          pinMode(pines[length-1], OUTPUT);
          digitalWrite(pines[length-1], HIGH);*/
          setPin(pines[length], 0);
          setPin(pines[length-1], 1);
          break;
        case 2:
          /*digitalWrite(pines[length], HIGH);
          pinMode(pines[length-2], OUTPUT);
          digitalWrite(pines[length-2], LOW);*/
          setPin(pines[length], 1);
          setPin(pines[length-2], 0);
          break;
        case 3:
          /*digitalWrite(pines[length], LOW);
          pinMode(pines[length-2], OUTPUT);
          digitalWrite(pines[length-2], HIGH);*/
          setPin(pines[length], 0);
          setPin(pines[length-2], 1);
          break;
      }
      delay(1);
      PORTA= PORTA & B01010000;//0x00;
      DDRA = DDRA & B01010000;//0x00;
      
    }
    if((segundo_digito>>i)&0x01){
      ////Serial.println(String("Comprobacion Realizada Bit numero: ")+i+String("Byte: ")+ segundo_digito);
      switch(i){
        case 0:
          if(horas){
            //pinMode(pines[0], OUTPUT);
            //pinMode(pines[1], OUTPUT);
            //digitalWrite(pines[0], HIGH);
            //digitalWrite(pines[1], LOW);
            setPin(pines[0], 1);
            setPin(pines[1], 0);
          }else{
            /*pinMode(pines[length-3], OUTPUT);
            pinMode(pines[length-4], OUTPUT);
            digitalWrite(pines[length-3], LOW);
            digitalWrite(pines[length-4], HIGH);*/
            setPin(pines[length-3], 0);
            setPin(pines[length-4], 1);
          }
          break;
        case 1:
          if(horas){
            /*pinMode(pines[0], OUTPUT);
            pinMode(pines[1], OUTPUT);
            digitalWrite(pines[0], LOW);
            digitalWrite(pines[1], HIGH);*/
            setPin(pines[0], 0);
            setPin(pines[1], 1);
          }
          else{
            /*pinMode(pines[length-3], OUTPUT);
            pinMode(pines[length-4], OUTPUT);
            digitalWrite(pines[length-3], HIGH);
            digitalWrite(pines[length-4], LOW);*/
            setPin(pines[length-3], 1);
            setPin(pines[length-4], 0);
          }
          break;
        case 2:
          /*pinMode(pines[length-4], OUTPUT);
          pinMode(pines[length-5], OUTPUT);
          digitalWrite(pines[length-4], not_minutes);
          digitalWrite(pines[length-5], !not_minutes);*/
          setPin(pines[length-4], not_minutes);
          setPin(pines[length-5], !not_minutes);
          break;
      }
      delay(1);
      PORTA= PORTA & B01010000;//0x00;
      DDRA = DDRA & B01010000;//0x00; //Excluimos los pines de I2C
    }

  }
}
void PbcdCounter(struct time_struct *t){
  //Positive Binary coded decimal counter. In this program, each byte has 2 decimals. Internet examples use only one per byte... such a waste in memory space
  //Lets try this only with seconds first...
  t->second++;
  //byte contra = (number & 0xF0)+0x0A;
  if(!((t->second&0x0F)^0x0A)){//if(!((number^0x0A)<<5)){//if(!((number&0x0F)^0x0A)){//if(!(number^contra)){
     t->second=(t->second&0xF0)+0x10;
     if(!((t->second&0xF0)^0x60)){
      t->second=0;
      t->minute++;
      if(!((t->minute&0x0F)^0x0A)){//if(!((number^0x0A)<<5)){//if(!((number&0x0F)^0x0A)){//if(!(number^contra)){
        t->minute=(t->minute&0xF0)+0x10;
        if(!((t->minute&0xF0)^0x60)){
          t->minute=0;
          t->hour++;
          if(!((t->hour&0x0F)^0x0A)){
            t->hour=(t->hour&0x30)+0x10;
          }
          else if(t->hour==0x24){
            t->hour=0;
          }
        }
      }
     }
  }
  /*if(((t->second&0x08)!=0x00)&&(t->second&0x02)!=0x00){
    t->second=(t->second&0xF0)+0x10;
  }*/
  //return number;
}

void loop() {
  //A ver... Los pines de los leds son el PFO, PF1, PF4, PF5, PF6. Con solo 5 pines podemos charliplexar 20 Leds. Ahora solo hay que conseguir escribir el codigo.
  //Con el registrp DDX, en este caso DDF, seleccionamos si deseamos el pin en estado salida o entrada. Si queremos que tenga una alta impedancia, lo seleccionamos a entrada, poniendolo
  //en  0, o en 1 si lo queremos como salida. 
  //En el registro PORTX, selecionamos el estado. Osea HIGH o LOW
  if(time.minute%10==0 && time.second==0){
    //Wire.begin();
    DS3231_get_time(&time);
  }
//  if(set_button_pressed()){
//    set_button_counter++;
//    unsigned long start=millis();
//    while(millis()>(start+TIMESET_TIMEOUT*1000)&set_button_counter==5){ //If the set button has been pressed for 5 secs, it enters into programming mode.
//      while(millis()-time_elapsed<1000){
//        printHoras(time.hour, Horas_pins, false, true);
//        printHoras(time.minute, Agrupacion_M, true, false);
//        printHoras(time.second, Agrupacion_S, false, false);
//        
//        }
//        time_elapsed=millis();
//        time.second++;
//        if (time.second ==60){
//          time.second=0;
//        }
//    }
//  }
//  else{
//    set_button_counter=0;
//  }
  selfsetHour();
  if(select_button_pressed()){
    int temperature=DS3231_get_temp();
    while(select_button_pressed()){
      printHoras(((temperature&0xFF00)>>8), Agrupacion_M, true, false);
      printHoras((temperature&0x00FF), Agrupacion_S, false, false);
      //printHoras((temperature), Agrupacion_M, true, false);
    }
  }
  while(millis()-time_elapsed<1000){
    printHoras(time.hour, Horas_pins, false, true);
    printHoras(time.minute, Agrupacion_M, true, false);
    printHoras(time.second, Agrupacion_S, false, false);
  }
  time_elapsed=millis();
  //time.second++;
  PbcdCounter(&time);
  /*if (time.second ==60){
    time.second=0;
    time.minute++;
    if(time.minute==60){
      time.minute=0;
      time.hour++;
      if(time.hour==24){
        time.hour=0;
        }
      }
    }*/
}
