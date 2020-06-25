/*
 * @Author: hzf
 * @Date: 2019-12-22 18:57:43
 */
#include<HX711.h>
#include<Servo.h>
#include <SoftwareSerial.h>
/*思路设计
 * 三色红绿灯模拟红绿灯，每隔一段时间改变红绿灯状态
 * 检测单位时间人的数量与车的数量，车流量大于人流量时，人对应的红绿灯时长加长，反之亦然，时间显示在数码管上
 * 光线暗的时候开灯，反之开灯
 * 舵机旋转打开人行道
 * 设定老年人按钮，延长人性绿灯时间
 * 蜂鸣器警告红灯状态，禁止通行
 * 
 */
SoftwareSerial mySerial(0, 1);   //RX, TX
 int num_per=0;//定义统计十秒内人流计数器
 int num_per_all=0;//定义人流总数量
 int num_car=0;//定义十秒内车流计数器
 int num_car_all=0;//定义车流总数量
 int tim_start=0;//定义初始时间
 int weight=0;
 int Carspeed=0;//定义车速
 bool isCar=0;//车的标志位
 bool isPer=0;//人的标志位
 int time_start=0;
 int time_end=0;
 int flag=0;//判断是否位车
 int RGB_P_start=0;//人行道红绿灯闪烁开始
// int RGB_P_end=0;//人行道红绿灯闪烁结束
 int RGB_C_start=0;//车行道红绿灯闪烁开始
 //int RGB_C_end=0;//车行道红绿灯闪烁结束
 int Per_Flow_start=0;//人流量统计开始
 int Car_Flow_start=0;//车流量统计开始
 int num_per_flow=0;//人流量
 int num_car_flow=0;//车流量
 int max_time=0;//红绿灯最大闪烁时间，车人分离时五秒/车人同时十秒

 void RGB_ON(int pin)//点亮RGB
{
  digitalWrite(pin, HIGH);
}
void RGB_OFF(int pin)//熄灭RGB
{
  digitalWrite(pin, LOW);
}

//统计十秒钟内的人流量
void GetFlow_Per()
{
  Serial.println("统计十秒内的车流量");
  isPer = digitalRead(A0);//检测到有人
  if(isPer)
  {
    num_per ++;
    num_per_all++;//总人流量
    Serial.print("当前十秒内经过的人数有");
    Serial.println(num_per);
    Serial.print("经过的总人数有");
    Serial.println(num_per_all);
  }
  
  if(num_per == 1)//首次检测到有人时
  {
     Per_Flow_start = millis();//开始计时
  }
  if(millis()-Per_Flow_start >= 10000)//十秒钟统计一次人流量
  {
    num_per_flow = num_per;
    Per_Flow_start = 0;
    num_per = 0;
  }
}

void GetFlow_Car()//统计十秒内的车流量,
{
  weight = -int(Get_Weight()) - 1300;
  if(weight < 0)
  {
    weight = -weight;
  }
  Serial.print("当前检测到的重量为：");
  Serial.println(weight);
  /*
   * 当第一次检测到有车时开始计时
   * 当距离第一次检测到有车十秒时获取十秒内的车流量
   * 将计时器以及车流量复位为0等待下一次计数
   */
  if(num_car == 1)
  {
    Car_Flow_start = millis();//车流量统计开始
  }
  if(millis-Car_Flow_start >= 10000)
  {
    num_car_flow = num_car;
    Car_Flow_start = 0;
    num_car = 0;
  }
  /*
   * 先检测一次压力>300判断为前轮
   * 再检测一个压力<300为前轮走过
   * 再检测到压力>300判断为后轮
   * 此时判断检测到车
   * 车的临时数量加一
   * 车的总数量加一
   */
  if(weight >= 300)//检测到压力
  {
    if(flag == 0)
    {
      time_start = millis();
      flag = 1;
    }
    if(flag == 2)
    {
      time_end = millis();
      flag = 3;
    }
  }
  else
  {
    if(flag == 1)
    {
      flag = 2;
    }
    if(flag == 3)
    {
       num_car ++;//车的临时数量加一，获得十秒钟内的车流量后复位为0
       num_car_all++;//车流总数量加一
       isCar = 1;//判断为车
       flag = 0;
       Serial.print("当前十秒内经过的车数有");
       Serial.println(num_car);
       Serial.print("经过的总车数有");
       Serial.println(num_car_all);
    }
  }
}

void MangreenBlink()
{
  //人行道绿灯闪烁
  digitalWrite(6,LOW);
  delay(500);
  digitalWrite(6,HIGH);
  delay(500);
  digitalWrite(6,LOW);
  delay(500);
  digitalWrite(6,HIGH);
  delay(500);
  digitalWrite(6,LOW);
  delay(500);
  digitalWrite(6,HIGH);
  delay(500);
  digitalWrite(6,LOW);

  digitalWrite(7,HIGH);
  delay(1500);
  digitalWrite(7,LOW);
}

void CargreenBlink()
{
  //车行道绿灯闪烁
  digitalWrite(3,LOW);
  delay(500);
  digitalWrite(3,HIGH);
  delay(500);
  digitalWrite(3,LOW);
  delay(500);
  digitalWrite(3,HIGH);
  delay(500);
  digitalWrite(3,LOW);
  delay(500);
  digitalWrite(3,HIGH);
  delay(500);
  digitalWrite(3,LOW);

  digitalWrite(4,HIGH);
  delay(1500);
  digitalWrite(4,LOW);
  
}


void setup() {
  // put your setup code here, to run once:
  //digitalWrite(2, HIGH);
  Serial.println("程序开始");

  pinMode(2, OUTPUT);//车行道红灯
  pinMode(3, OUTPUT);//车行道绿灯
  pinMode(4, OUTPUT);//车行道黄灯

  pinMode(5, OUTPUT);//人行道红灯
  pinMode(6, OUTPUT);//人行道绿灯
  pinMode(7, OUTPUT);//人行道黄灯

  pinMode(A0, INPUT);//人体红外检测

  Get_Maopi();
  Init_Hx711();

  Serial.begin(9600); 
  mySerial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  GetFlow_Per();//判断是否为人同时计算人流量
  GetFlow_Car();//判断是否为车同时计算车流量
  Serial.print("isPer = ");
  Serial.println(isPer);
  Serial.print("isCar = ");
  Serial.println(isCar);

  if(RGB_P_start)//假设人行道红绿灯开始计时
  {
    if(millis() - RGB_P_start >= max_time)//绿灯闪烁超过最大闪烁时间后
    {
      MangreenBlink();//人行道绿灯闪烁
      Serial.println("绿灯亮的时间超过max_time,人行道绿灯闪烁");
      RGB_P_start = 0;
    }
  }

  if(RGB_C_start)//假设人行道红绿灯开始计时
  {
    if(millis() - RGB_C_start >= max_time)//绿灯闪烁超过最大闪烁时间后
    {
      CargreenBlink();//车行道绿灯闪烁
      Serial.println("绿灯亮的时间超过max_time,车行道绿灯闪烁");
      RGB_C_start = 0;
    }
  }

  if(isPer && isCar == 0)
  {
    mySerial.write(1);
    RGB_ON(6);//人行道绿灯开启
    RGB_ON(2);//车行道红灯开启
    RGB_P_start = millis();//人行道绿灯开始计时
    max_time = 5000;//最大闪烁时间
    
    RGB_OFF(3);
    RGB_OFF(4);
    RGB_OFF(5);
    RGB_OFF(7);
  }

  else if(isPer == 0 && isCar == 1)
  {
     mySerial.write(2);
    RGB_ON(5);//人行道红灯开启
    RGB_ON(3);//车行道绿灯开启
    RGB_C_start = millis();//车行道绿灯开始计时
    max_time = 5000;//最大闪烁时间
    
    RGB_OFF(2);
    RGB_OFF(4);
    RGB_OFF(6);
    RGB_OFF(7);
  }

  else if(isPer == 0 && isCar == 0)
  {
     mySerial.write(2);
    RGB_ON(2);//车行道红灯开启
    RGB_ON(5);//人行道红灯开启
    RGB_C_start = millis();//人行道红灯开始计时
    max_time = 5000;//最大闪烁时间
    
    RGB_OFF(3);
    RGB_OFF(4);
    RGB_OFF(6);
    RGB_OFF(7);
  }

  else if(isPer == 1 && isCar == 1)
  {
    if(num_per_flow < num_car_flow)//十秒内的车流量小于人流量
    {
       mySerial.write(1);
      RGB_ON(2);//车行道红灯开启
      RGB_ON(6);//人行道绿灯开启
      RGB_C_start = millis();//人行道红灯开始计时
      max_time = 10000;//最大闪烁时间
      
      RGB_OFF(3);
      RGB_OFF(4);
      RGB_OFF(6);
      RGB_OFF(7);
    }
    else if(num_per_flow > num_car_flow)//十秒内的车流量大于人流量
    {
      RGB_ON(5);//人行道红灯开启
      mySerial.write(2);
      RGB_ON(3);//车行道绿灯开启
      RGB_C_start = millis();//人行道绿灯开始计时
      max_time = 10000;//最大闪烁时间
      
      RGB_OFF(2);
      RGB_OFF(4);
      RGB_OFF(6);
      RGB_OFF(7);
    }
  }

}
