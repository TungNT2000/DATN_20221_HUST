#include <Arduino.h>
#include "DHT.h"
#include <M_DEFINE.h>
#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <M_WIFI.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); 
DHT dht(DHT_PIN, DHTTYPE);
StaticJsonDocument<250> docIn;
StaticJsonDocument<250> docOut;
String jsonDataSend;
String jsonDataRcv;
uint32_t mqtt_now;
bool pir1On,pir2On,pir3On;
float hvalue,tvalue;
uint32_t gasValue;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int mqtt_interval=5000;
void mqtt_setup();
void mqtt_connect_to_broker();
void mqtt_callback(char* topic, byte *payload, unsigned int length);
void mqtt_loop();
void mqtt_public(char* pubTopic,bool rl);
//fb
FirebaseData ledData;
FirebaseJson json;
// bool fbChangeState;// flag for firebase
// bool mqttChangeState;// flag for mqtt

uint64_t now_tmp;
uint64_t now_pir,now_pir1,now_pir2,now_pir3;
uint64_t now;
uint8_t cnt1t,cnt1f;
uint8_t cnt2t,cnt2f;
uint8_t cnt3t,cnt3f;
bool toggle_state(bool s){
  return !s;
}
//FB
void firebase_setup(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  now=millis();
}
void mqtt_setup(){
  client.setServer(MQTT_SERVER, MQTT_PORT );
  client.setCallback(mqtt_callback);
  mqtt_now=millis();
}
void mqtt_connect_to_broker(){
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish(MQTT_PUB_SM_SW_R3_TOPIC, "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_RL1_SUB);
      client.subscribe(MQTT_RL2_SUB);
      client.subscribe(MQTT_RL3_SUB);
    //  client.subscribe(MQTT_RL2_IN_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
  }
}

void mqtt_callback(char* topic, byte *payload, unsigned int length){
  char* buff=(char*)payload;
    jsonDataRcv= String(buff);
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println( jsonDataRcv);
    DeserializationError err =deserializeJson(docIn,jsonDataRcv);
  // Switch on the LED if an 1 was received as first character
      if(!err){
       // bool tmp;
          // if(strcmp(topic,MQTT_RL1_SUB)==0){
          //    tmp=docIn["ON"];
          //    if(tmp!=rl1On){
          //     rl1On=tmp;
          //     digitalWrite(RL1_PIN,rl1On);
          //     Firebase.setBool(ledData,FIREBASE_RL1,rl1On);
          //    }
          // }else if(strcmp(topic,MQTT_RL2_SUB)==0){
          //     tmp=docIn["ON"];
          //     if(tmp!=rl2On){
          //     rl2On=tmp;
          //     digitalWrite(RL2_PIN,rl2On);
          //     Firebase.setBool(ledData,FIREBASE_RL2,rl2On);
          //    }
          //  }else if(strcmp(topic,MQTT_RL3_SUB)==0){
          //     tmp=docIn["ON"];
          //     if(tmp!=rl3On){
          //     rl3On=tmp;
          //     digitalWrite(RL3_PIN,rl3On);
          //     Firebase.setBool(ledData,FIREBASE_RL3,rl3On);
          //    }
             // }
    } else{
        Serial.println("false");
        Serial.println(err.f_str());
        }
}
void mqtt_loop(){
  client.loop();
    if (!client.connected()&& millis()-mqtt_now>mqtt_interval) {
        mqtt_connect_to_broker();
        mqtt_now=millis();
        }
    client.loop();
}
void mqtt_public(char*Topic,bool rl){
  jsonDataSend="";
  docOut.clear();
    docOut["ON"]=rl;
    serializeJson(docOut,jsonDataSend);
     Serial.println("pub");
     Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void mqtt_publi_int(char*Topic,float i){
  jsonDataSend="";
  docOut.clear();
    i=i;
    docOut["VAL"]=i;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void sensor_setup(){
   dht.begin();
   pinMode(GAS_PIN,INPUT);
   pinMode(PIR1_PIN,INPUT);
   pinMode(PIR2_PIN,INPUT);
   pinMode(PIR3_PIN,INPUT);
  lcd.init();                 
  lcd.backlight();
   lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, HUST");
}
void setup() {
  Serial.begin(115200);
 // pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
  sensor_setup();
  wifi_setup();
  mqtt_setup();
  firebase_setup();
 
  now_tmp=millis();
   now_pir=millis();
  now_pir1=millis();
  now_pir2=millis();
  now_pir3=millis();
}
bool tag1=true,tag2=false,tag3=false;
void loop() {
  mqtt_loop();
  if(millis()-now_tmp>2000){
    lcd.clear();
      float h= dht.readHumidity();
      float t= dht.readTemperature();
      Serial.println(h);
      Serial.println(t);
      if (isnan(h) || isnan(t)) {
          lcd.setCursor(0, 0);
          lcd.setCursor(0, 0);  
          lcd.print("Temp: ");
          lcd.print("Failed");    
          lcd.print(" *C");

          lcd.setCursor(0, 1);  
          lcd.print("Humi: ");
          lcd.print("Failed");      
          lcd.print(" %");
        } else {
          lcd.setCursor(0, 0);  // display position
          lcd.print("Temp: ");
          lcd.print(t);     // display the temperature
          lcd.print("°C");

          lcd.setCursor(0, 1);  // display position
          lcd.print("Humi: ");
          lcd.print(h);      // display the humidity
          lcd.print("%");
          mqtt_publi_int(MQTT_DHT_TEM_PUB,t);
          mqtt_publi_int(MQTT_DHT_HUM_PUB,h);
          Firebase.setInt(ledData,FIREBASE_DHT,t);
        }
      uint32_t gasValue=analogRead(GAS_PIN);
      // if(abs(h-hvalue)<0.01){
        

      // }
      // uint8_t percent =map(gasValue,0,4096,0,100);
     // Serial.println(percent);
      Serial.println("gas");
      mqtt_publi_int(MQTT_GAS_PUB,gasValue);
      Serial.println("dht");
      Firebase.setInt(ledData,FIREBASE_DHT,t);
      now_tmp=millis();
  }
   if((millis()-now_pir>200) && tag1==true){
      bool tmp1=digitalRead(PIR1_PIN);
      Serial.print("tmp1= ");
      Serial.println(tmp1);
    //  bool tmp2=digitalRead(PIR2_PIN);
    //  bool tmp3=digitalRead(PIR3_PIN);
    if(tmp1==0){
      cnt1t=0;
      cnt1f++;
     if(cnt1f==4){
        if(pir1On==true){
          Serial.println("pir 1 f ok");
          pir1On=false;
          mqtt_public( MQTT_PIR1_PUB,pir1On);
        }
        cnt1f=0;
      }
    } else if(tmp1==1){
      cnt1t ++;
      cnt1f=0;
      if(cnt1t==4){
        if(pir1On==false){
          Serial.println("pir 1 t ok");
          pir1On=true;
          mqtt_public( MQTT_PIR1_PUB,pir1On);
        }
        cnt1t=0;
      }
    }
    
    // if(tmp1!=pir1On){
    //   mqtt_public( MQTT_PIR1_PUB,tmp1);
    //   pir1On=tmp1;
    // }
    // if(tmp2!=pir2On){
    //    pir2On=tmp2;
    //   mqtt_public( MQTT_PIR2_PUB,tmp2);
     
    //   Serial.println("pir 2 =   ");
    //   Serial.println(pir2On);
    // }
   
    // if(tmp3!=pir3On){    
    //   pir3On=tmp3;
    //   mqtt_public( MQTT_PIR3_PUB,tmp3);
     
    //   Serial.println("pir 3 =   ");
    //   Serial.println(pir3On);
    // }
    tag1=false;
    tag2=true;
    now_pir=millis();
  }
  if((millis()-now_pir>200)&&tag2==true){
      bool tmp2=digitalRead(PIR2_PIN);
      Serial.print("tmp2= ");
      Serial.println(tmp2);
    if(tmp2==0){
      cnt2t=0;
      cnt2f++;
     if(cnt2f==4){
        if(pir2On==true){
          Serial.println("pir 2 f ok");
          pir2On=false;
          mqtt_public( MQTT_PIR2_PUB,pir2On);
        }
        cnt2f=0;
      }
    } else if(tmp2==1){
      cnt2t ++;
      cnt2f=0;
      if(cnt2t==4){
        if(pir2On==false){
          Serial.println("pir 2 t ok");
          pir2On=true;
          mqtt_public( MQTT_PIR2_PUB,pir2On);
        }
        cnt2t=0;
      }
    }
    tag2=false;
    tag3=true;
    now_pir=millis();
  }
  if((millis()-now_pir3>200)&& tag3==true){
      bool tmp3=digitalRead(PIR3_PIN);
      Serial.print("tmp3= ");
      Serial.println(tmp3);
    if(tmp3==0){
      cnt3t=0;
      cnt3f++;
     if(cnt3f==4){
        if(pir3On==true){
          Serial.println("pir 3 f ok");
          pir3On=false;
          mqtt_public( MQTT_PIR3_PUB,pir3On);
        }
        cnt3f=0;
      }
    } else if(tmp3==1){
      cnt3t ++;
      cnt3f=0;
      if(cnt3t==4){
        if(pir3On==false){
          Serial.println("pir 3 t ok");
          pir3On=true;
          mqtt_public( MQTT_PIR3_PUB,pir3On);
        }
        cnt3t=0;
      }
    }
    tag3=false;
    tag1=true;
    now_pir=millis();
  }
}