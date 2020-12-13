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

//Switch from CM to NCM
void REG_write_CM(void){ 
  Wire.beginTransmission(Addr);
  Wire.write(0x02);  //  選擇02h
  Wire.write(0x02);  //  寫入值
  Wire.write(0x00);  
  Wire.write(0x08);
  Wire.write(0x00);
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

  //REG_write(0x16,0x09);  delay(100);
  REG_write(0x16,0x00);  delay(100);
  //REG_write(0x16,0x12);  delay(100);
}

void threshold(int a){
  REG_write(0x03,a);
}

//Motion Detection Function
void motion(void){
  threshold(10);
}

void setup()
{
  delay(100);
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Serial_begin");
  rinit();
  //REG_write_CM();
  delay(100);
  Serial.println("init_done");
  delay(100);
}

void loop()
{
  REG_readm(0x04);
  int x = (arr[3]|((arr[4]<<24)>>16))>>1;
  int y = (arr[5]|((arr[6]<<24)>>16))>>1;
  int z = arr[7]|((arr[8]<<24)>>16);
  Serial.print("x=");Serial.print(x);Serial.print(" , ");Serial.print("y=");Serial.print(y);Serial.print(" , ");Serial.print("z=");Serial.print(z);
  
  //acceleration
  /*float xg=x;
  float yg=y;
  float zg=z;
  xg=xg/256;
  yg=yg/256;
  zg=zg/256;
  Serial.print("x=");Serial.print(xg);Serial.print(" g, ");Serial.print("y=");Serial.print(yg);Serial.print(" g, ");Serial.print("z=");Serial.print(zg);Serial.println(" g");*/

  Serial.println();
  delay(100);
}