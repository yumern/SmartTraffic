#include<Servo.h>
#include <SoftwareSerial.h>
Servo S;
int flag_g = 0;//光敏
 int flag = 0;//串口数据

SoftwareSerial mySerial(6, 7);
void setup() {
  // put your setup code here, to run once:
  pinMode(A7, INPUT);
  pinMode(5, OUTPUT);
  S.attach(9);
   digitalWrite(2, HIGH);
  mySerial.begin(115200);
  Serial.begin(9600);


}

void loop() {
  // put your main code here, to run repeatedly:
  flag_g = digitalRead(A7);
  if(flag_g == 1)
  {
    Serial.print("开灯:    ");
    Serial.println(flag_g);
    digitalWrite(5, HIGH);
  }
  else
  {
    Serial.print("关灯：   ");
    Serial.println(flag_g);
    digitalWrite(5,HIGH);
  }
  if(mySerial.available())
  {
    Serial.print("flag的值为：");
    flag = mySerial.read();
    Serial.println(flag_g);
    switch(int(flag))
    {
      case 1:
      {
        Serial.println("打开人行道");
        S.write(80);
        break;
      }
      case 2:
      default:
      {
        Serial.println("关闭人行道");
        S.write(0);
        break;
      }
    }
  }
}
