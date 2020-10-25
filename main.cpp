#include <Arduino.h>

#include <Wire.h>
#include <math.h>

#define Addr 0x19 //  I2C address

int arr[11];

void REG_write(int a,int b){ 
  Wire.beginTransmission(Addr);
  Wire.write(a);  //  選擇暫存器
  Wire.write(b);  //  寫入值
  Wire.endTransmission();  
}

int REG_read1(int a){
  int data1;
  Wire.beginTransmission(Addr);
  Wire.write(a);  //  選擇暫存器
  Wire.endTransmission(0);  
  Wire.requestFrom(Addr, 1); //request 1 byte
  data1 = Wire.read(); //read data
  return data1;
}
void REG_readm(int a){
  Wire.beginTransmission(Addr);
  Wire.write(a);  //  選擇暫存器
  Wire.endTransmission(0);  
  Wire.requestFrom(Addr, 11); //request 11 bytes
  int c=0;
  while(Wire.available()){
    arr[c]=Wire.read();
    c++;
  }
  Serial.println();
}
void rinit(void){
  REG_write(0x01,2);
  if(REG_read1(0x00)!=0xA3){
    Serial.println("i2c error");
    return ;
  }

  REG_write(0x18,0x40);  
  delay(100);
  Serial.println("0x18=");Serial.println(REG_read1(0x18));
  REG_write(0x15,0x2A);  
  delay(100);
  Serial.println("0x15=");Serial.println(REG_read1(0x15));
  REG_write(0x16,0x09);  //low-pass filter
  delay(100);
  Serial.println("0x16=");Serial.println(REG_read1(0x16));

}
void threshold(int a){
  REG_write(0x03,a);
}
void motion(void){//Motion Detection Function
  threshold(10);
}

void setup()
{
  delay(100);
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Serial_begin");
  //rinit();
  Serial.println("init_done");
  delay(100);
  //motion();
  Serial.println("motion_done");
  delay(100);
}

void loop()
{
  REG_readm(0x04);
  int x = (arr[3]>>1)|((arr[4]<<24)>>17);
  int y = (arr[5]>>1)|((arr[6]<<24)>>17);
  int z = arr[7]|((arr[8]<<24)>>16);
  float a=x;
  float b=y;
  float c=z;
  a=a/256;
  b=b/256;
  c=c/256;
  Serial.print("x=");Serial.print(a);Serial.print(" g, ");Serial.print("y=");Serial.print(b);Serial.print(" g, ");Serial.print("z=");Serial.print(c);Serial.println(" g");
  if((x>150)||(x<(-150))||(y>150)||(y<(-150))||(z>150)||(z<(-150))){
    Serial.println("over");
  }
  Serial.println();
  delay(100);
}