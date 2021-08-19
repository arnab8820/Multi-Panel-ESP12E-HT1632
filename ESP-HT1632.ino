#include <ESPWiFiManager.h>
#include <HT1632.h>
#include <AsyncPing.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "DHT.h"

//DISPLAY CONNECTION CONFIG 
#define CS D1
#define WR D0
#define DATA D2
//END DISPLAY CONNECTION

//DHT CONFIG
#define DHTPIN D4
#define DHTTYPE DHT11

//configuration
//IPAddress ip (13, 126, 86, 106); // The remote ip to ping
IPAddress ip (8, 8, 8, 8); // The remote ip to ping
const long utcOffsetInSeconds = 19800;

//object declaration
ESPWiFiManager wifi;
AsyncPing ping;
DHT dht(DHTPIN, DHTTYPE);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

//global variables
int h=0, m=0;
boolean dot=false, pingRecvd=false;
int runningLed = 0;

unsigned long int clockTimer, dotTimer, runningTimer;

void getNtpTime(){
  timeClient.update();
  h = timeClient.getHours();
  if(h==0){
    h=12;
  } else if(h>12){
    h = h - 12;
  }
  m = timeClient.getMinutes();
}

void keepTime(){
  if(millis()-clockTimer>=60000){
    clockTimer = millis();
    m++;
    if(m>=60){
      h++;
      m=0;
    }
    if(h>12){
      h=1;
    }
    displayClimate();
  }
}

void displayTime(){
  displayDigit(0, h/10);
  displayDigit(1, h%10);
  displayDigit(2, m/10);
  displayDigit(3, m%10);
}

void displayPing(int tm){
  int pos = 8;
  clearDigit(5);
  clearDigit(6);
  clearDigit(7);
  clearDigit(8);
  while(tm!=0){
    displayDigit(pos, tm%10);
    tm=tm/10;
    pos--;
  }
}

void displayPingTimeout(){
  clearDigit(5);
  clearDigit(6);
  clearDigit(7);
  clearDigit(8);
  HT1632.setPixel(5, 1);
  HT1632.setPixel(6, 1);
  HT1632.setPixel(7, 1);
  HT1632.setPixel(8, 1);
  HT1632.render();
}

void displayConnectionStatus(int tm){
  HT1632.clearPixel(22, 2);
  HT1632.clearPixel(22, 3);
  HT1632.clearPixel(22, 4);
  if(tm<100){
    HT1632.setPixel(22, 4);
  } else if(tm>=100&&tm<500){
    HT1632.setPixel(22, 3);
  } else {
    HT1632.setPixel(22, 2);
  }
  HT1632.render();
}

void displayClimate(){
  dht.read();
  int temp = dht.readTemperature()*10;
  int hum = dht.readHumidity();
  int pos;
  Serial.printf("Temperature: %d Humidity: %d\n", temp, hum);
  for(pos=15; pos<21; pos++){
    clearDigit(pos);
  }
  if(!isnan(temp)){
    pos = 20;
    while(temp!=0){
      displayDigit(pos, temp%10);
      temp=temp/10;
      pos--;
    }
  }
  HT1632.setPixel(19, 7); //set decimal point
  if(!isnan(hum)){
    pos = 17;
    while(hum!=0){
      displayDigit(pos, hum%10);
      hum=hum/10;
      pos--;
    }
  }
  
}

void handleDot(){
  if(millis()-dotTimer>=500){
    dotTimer = millis();
    dot = !dot;
    if(dot){
      HT1632.setPixel(4, 0);
      HT1632.setPixel(4, 6);
    } else {
      HT1632.clearPixel(4, 0);
      HT1632.clearPixel(4, 6);
    }
  }
}

void handleRunning(){
  if(millis()-runningTimer>=1000){
    runningTimer = millis();
    switch(runningLed){
      case 0: 
        HT1632.clearPixel(21, 7);
        HT1632.setPixel(21, 0);
        break;
      case 1: 
        HT1632.clearPixel(21, 0);
        HT1632.setPixel(21, 1);
        break;
      case 2: 
        HT1632.clearPixel(21, 1);
        HT1632.setPixel(21, 2);
        break;
      case 3: 
        HT1632.clearPixel(21, 2);
        HT1632.setPixel(21, 3);
        break;
      case 4: 
        HT1632.clearPixel(21, 3);
        HT1632.setPixel(21, 4);
        break;
      case 5: 
        HT1632.clearPixel(21, 4);
        HT1632.setPixel(21, 5);
        break;
      case 6: 
        HT1632.clearPixel(21, 5);
        HT1632.setPixel(22, 0);
        break;
      case 7: 
        HT1632.clearPixel(22, 0);
        HT1632.setPixel(22, 1);
        break;
      case 8: 
        HT1632.clearPixel(22, 1);
        HT1632.setPixel(21, 6);
        break;
      case 9: 
        HT1632.clearPixel(21, 6);
        HT1632.setPixel(21, 7);
        break;
    }
    if(runningLed==9){
      runningLed = 0;
    } else {
      runningLed++;
    }
    if(pingRecvd){
      ping.begin(ip, 1);
      pingRecvd=false;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initDisplay();
  for(int x=0; x<21; x++){
    HT1632.setPixel(x, 1);
  }
  HT1632.setPixel(4, 0);
  HT1632.setPixel(4, 6);
  HT1632.render();
  dht.begin();
  wifi.initWifiManager();
  initOta();
  getNtpTime();
  clockTimer=millis();
  dotTimer=millis();
  runningTimer=millis();

  ping.on(true, [](const AsyncPingResponse& response){
    if(response.answer){
      pingRecvd=true;
      displayPing(response.time);  
      displayConnectionStatus(response.time);
    } else {
      //request timeout happened
      pingRecvd=true;
      displayPingTimeout();
      displayConnectionStatus(1000);
    }
    return true;
  });
//  ping.on(false, [](const AsyncPingResponse& response){
//    if(response.answer){
//      displayPing(response.time);  
//    }
//    return false;
//  });
  ping.begin(ip, 1);
  
  HT1632.setBrightness(16);
  HT1632.clear();
  displayClimate();
}

void loop() {
  otaHandler();
  keepTime();
  displayTime();
  handleDot();
  handleRunning();
}
