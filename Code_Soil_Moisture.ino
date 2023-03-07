#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[]= "";
char ssid[]= ""; //Tên wifi
char pass[]= ""; // Mật khẩu wifi

int Ref, sw_mode, button1, button2, sensor_value;

const int trigPin = 12; //D6
const int echoPin = 14; //D5
//define sound velocity in cm/uS

#define DHTPIN 2 //D4
#define DHTTYPE DHT11 //Dùng loại DHT11
#include <DHT.h>
DHT dht(DHTPIN, DHTTYPE);

long duration;
float distanceCm;

WidgetLED led(V0);//Led Connect
WidgetLED appledr(V5);
WidgetLED appledg(V6);
WidgetLCD hienthi(V7); //Hiển thị trạng thái trên LCD app Blynk

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(trigPin, OUTPUT);//Xuất tín hiệu ra
  pinMode(echoPin, INPUT);//Thu tín hiệu vào
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  
}

BLYNK_WRITE(V3){//Ngưỡng
  Ref = param.asInt();
}

BLYNK_WRITE(V4){//Chế độ Auto hay Manual
  sw_mode = param.asInt();
}

BLYNK_WRITE(V8){
  button1 = param.asInt();
}

BLYNK_WRITE(V9){
  button2 = param.asInt();
}

void loop() {
  Blynk.run();
  //Hàm hiển thì trạng thái bật tắt bơm chế độ auto
  String s;
  if(sensor_value<Ref){
    s = "Bơm bật";
  }else{
    s="Bơm tắt";
  }
  Blynk.virtualWrite(V10,s);
  //Xử lý tín hiệu Cảm biến Soil Moisture
  sensor_value = analogRead(A0);
  sensor_value = map(sensor_value, 0, 1023, 0, 100);//Chuyển về dạng 100%
  Blynk.virtualWrite(V1,sensor_value);
  //Xử lý tín hiệu cảm biến DHT
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = 0.034 * duration/2;
  Blynk.virtualWrite(V2,distanceCm);
  //Hiển thị trạng thái Kết nối VXL với App Blynk
  if(led.getValue()) {
    led.off();
    delay(1000);
  }
  else {
    led.on();
    delay(1000);
  }
  //***********************************************
  if(sw_mode == 0){//Chế độ Auto
    if(sensor_value<Ref){
      appledr.on();
      hienthi.print(0 , 0, "Độ ẩm:" + String(sensor_value) + "%    ");
      hienthi.print(0 , 1, "Mực nước:" + String(distanceCm) + "cm ");
      digitalWrite(D1,HIGH);
      //digitalWrite(D2,HIGH);
    }
    else{
      appledr.off();
      hienthi.print(0 , 0, "Độ ẩm:" + String(sensor_value) + "%    ");
      hienthi.print(0 , 1, "Mực nước:" + String(distanceCm) + "cm ");
      digitalWrite(D1,LOW);
      //digitalWrite(D2,LOW);
    }
    if (distanceCm < 4){
      appledg.on();
      digitalWrite(D2,LOW);
    }
    else{
      appledg.off();
      digitalWrite(D2,HIGH);
    }
  }
  else{//Chế độ Manual
    if(button1 == 1){
      appledg.on();
      hienthi.print(0 , 0, "Bơm 1 đang bật");
      digitalWrite(D1,HIGH);
    }
    else{
      appledg.off();
      hienthi.print(0 , 0, "Bơm 1 đang tắt");
      delay(2000);
      digitalWrite(D1,LOW);
    }

    if(button2 == 1){
      appledr.on();
      hienthi.print(0 , 1, "Bơm 2 đang bật ");
      digitalWrite(D2,HIGH);
    }
    else{
      appledr.off();
      hienthi.print(0 , 1, "Bơm 2 đang tắt ");
      digitalWrite(D2,LOW);
    }
  }
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Blynk.virtualWrite(V11,h);
  Blynk.virtualWrite(V12,t);
}
