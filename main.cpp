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

//sensor data output
void REG_readm(int a){
  Wire.beginTransmission(Addr);
  //  choose Register a
  Wire.write(a); 
  Wire.endTransmission(0);  
  //request 11 bytes and save them in arr[]
  Wire.requestFrom(Addr, 11); 
  int c=0;
  while(Wire.available()){
    arr[c]=Wire.read();
    c++;
  }
}

void rinit(void){

  /*Power-on reset*/
  REG_write(0x01,2);  delay(100);

  /*Read the Register 00h (PID) for product ID. The PID value should be 0xA3. Check the
  I2C connections if the report value is otherwise. */
  if(REG_read1(0x00)!=0xA3){
    Serial.println("i2c error");
    return ;
  }

  /* turn on the offset temperature compensation. */
  REG_write(0x18,0x40);  delay(100);

  /*turn on the data ready interrupt and configure the
  INT pin to active high, push-pull type*/
  REG_write(0x15,0x2A);  delay(100);

  /*Write 0x09 to Register 16h. This will turn on the internal low-pass filter. Depending on
  the application, user may set 0x00 to Register 16h to turn it off, or set 0x12 to Register 16h to
  turn on the high-pass filter instead. */
  REG_write(0x16,0x09);  delay(100);

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
  rinit();
  Serial.println("init_done");
  delay(100);
}

void loop()
{
  REG_readm(0x04);
  int x = (arr[3]>>1)|((arr[4]<<24)>>17);
  int y = (arr[5]>>1)|((arr[6]<<24)>>17);
  int z = arr[7]|((arr[8]<<24)>>16);
  float xg=x;
  float yg=y;
  float zg=z;
  xg=xg/256;
  yg=yg/256;
  zg=zg/256;
  Serial.print("x=");Serial.print(xg);Serial.print(" g, ");Serial.print("y=");Serial.print(yg);Serial.print(" g, ");Serial.print("z=");Serial.print(zg);Serial.println(" g");
  if((x>150)||(x<(-150))||(y>150)||(y<(-150))||(z>150)||(z<(-150))){
    Serial.println("over");
  }
  Serial.println();
  delay(100);
}