#define DS3231_I2C_ADDR 0x68
#define DS3231_TEMP_ADDR 0x11
#define DS3231_ALARM1_ADDR 0x07
#define DS3231_ALARM2_ADDR 0x0B
#define DS3231_TIME_ADDR 0x00
#define DS3231_STATUS_ADDR 0x0F 
#define DS3231_STATUS_EN32KHZ 0x08
#define DS3231_TRANSACTION_TIMEOUT 100


void resetI2C(){
  //Sacado de https://github.com/puuu/USIWire/blob/master/src/USI_TWI_Master/USI_TWI_Master.c
  //Reseteamos el estado de los Pines, porque los ponemos a 0, cada vez que mostramos la pantalla.
  PORT_USI |= (1 << PIN_USI_SDA); // Enable pullup on SDA, to set high as released state.
  PORT_USI_CL |= (1 << PIN_USI_SCL); // Enable pullup on SCL, to set high as released state.

  DDR_USI_CL |= (1 << PIN_USI_SCL); // Enable SCL as output.
  DDR_USI |= (1 << PIN_USI_SDA); // Enable SDA as output.
}
void DS3231_set_time(struct time_struct *t){
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TIME_ADDR);
  Wire.write(t->second);
  Wire.write(t->minute);
  Wire.write(t->hour);
  Wire.endTransmission();
}
int DS3231_get_temp(){
  //resetI2C();
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TEMP_ADDR);
  Wire.endTransmission(); 
  //if(0!=Wire.endTransmission()) return 0x22;
  uint8_t gotData = false;
  uint32_t start = millis();
  while(millis()-start<DS3231_TRANSACTION_TIMEOUT){
    if(Wire.requestFrom(DS3231_I2C_ADDR, 2)==2){
      gotData =true;
      break;
    }
  }
  if(!gotData){
    return;
  }
  uint8_t temp_msb = Wire.read();
  uint8_t temp_lsb = Wire.read() >> 6;
  int8_t nint;
  if((temp_msb & 0x80) !=0) nint = temp_msb | ~((1<<8) -1);
  else nint = ((temp_msb/10)<<4)+(temp_msb%10);
  //return 0.25*temp_lsb + nint;
  return temp_lsb + (nint<<8);
  //temp_msb=25;
  //return temp_msb;
}
void DS3231_get_time(struct time_struct *t){
  //resetI2C();
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(DS3231_TIME_ADDR);
  Wire.endTransmission();
  uint8_t gotData = false;
  uint32_t start= millis();
  while(millis()-start < DS3231_TRANSACTION_TIMEOUT){
    if(Wire.requestFrom(DS3231_I2C_ADDR, 3)==3){
      gotData=true;
      break;
    }
  }
  
  if (!gotData){
    //return;
      DDRA |= (0X01 << 2);
      PORTA |= (1 << 2);
      DDRA |= (0X01 << 1);
      PORTA |= (0 << 1);
    while(true){
      //Sit here to notice the error
    }
  }
  //uint8_t n;
  uint8_t Time[3];
  for(uint8_t i=0; i<=2; i++){
    //n = Wire.read();
    //Time[i]=bcdtodec(n);
    Time[i]=Wire.read();
  }
  t->second = Time[0];  //If we just save the data as bcd, then we dont need to do a later conversion
  t->minute = Time[1];
  t->hour   = Time[2];
  Wire.endTransmission();
}
void DS3231_init(){
  //resetI2C();
  uint8_t sreg = DS3231_get_addr(DS3231_STATUS_ADDR);
  sreg &= !DS3231_STATUS_EN32KHZ;
  DS3231_set_addr(DS3231_STATUS_ADDR, sreg);
}
void DS3231_set_addr(uint8_t addr, uint8_t val){
  //resetI2C();
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission();
}
uint8_t DS3231_get_addr(uint8_t addr){
  //resetI2C();
  Wire.beginTransmission(DS3231_I2C_ADDR);
  Wire.write(addr);
  Wire.endTransmission();
  uint8_t rv;
    uint8_t gotData = false;
  uint32_t start = millis(); // start timeout
  while(millis()-start < DS3231_TRANSACTION_TIMEOUT){
    if (Wire.requestFrom(DS3231_I2C_ADDR, 1) == 1) {
        gotData = true;
        break;
      }
      delay(2);
    }
  Wire.endTransmission();
  if (!gotData)
      return 0; // error timeout
    rv = Wire.read();
    return rv;
}
byte bcdtodec(byte val){
  return((val/16*10)+(val%10));
}
