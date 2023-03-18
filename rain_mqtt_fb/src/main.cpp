#include <Arduino.h>
#include<M_DEFINE.h>
#include<FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include<OneButton.h>

dc_state_t dc;
// btn
OneButton btn1(BTN1_PIN,false,false);//< 1
OneButton btn2(BTN2_PIN,false,false);//|| 0
OneButton btn3(BTN3_PIN,false,false);//> 2
//OneButton btnRst(BTN_RESET_PIN,true);
bool mqttPub=false;
bool fbPub =false;
bool isOpen=false;
bool notRain =true;
void relay_to_left(){
  digitalWrite(RL1_PIN,HIGH);
  digitalWrite(RL2_PIN,LOW);
}
void relay_to_right(){
  digitalWrite(RL2_PIN,HIGH);
  digitalWrite(RL1_PIN,LOW);
}
void relay_to_stop(){
  digitalWrite(RL2_PIN,HIGH);
  digitalWrite(RL1_PIN,HIGH);
}
void on_btn1(){
  Serial.println("btn1_ok");
  if(dc!=TO_LEFT){
    if(digitalRead(LM1_PIN)==HIGH){
        dc=TO_LEFT;
        Serial.println("to left");
        relay_to_left();
        isOpen=true;
    }
  }

}
void on_btn2(){
  Serial.println("btn2_ok");
   if(dc!=TO_PAUSE){
      dc=TO_PAUSE;
      Serial.println("to pause");
      relay_to_stop();
       isOpen=true;
       mqttPub=true;
       fbPub=true;
   }
  
}
void on_btn3(){
 // if(notRain){
  Serial.println("btn3_ok");
   if(dc!=TO_RIGHT){
    if(digitalRead(LM2_PIN)==HIGH){
        dc=TO_RIGHT;
        Serial.println("to right");
         relay_to_right();
         isOpen=true;
      }
    }
 // }
}
void relay_setup(){
    pinMode(RL1_PIN,OUTPUT);
    pinMode(RL2_PIN,OUTPUT);
}
void btn1_on_click(){
  on_btn1();
}
void btn2_on_click(){
  on_btn2();
}
void btn3_on_click(){
  on_btn3();
}
void button_loop(){
  btn1.tick();
  btn2.tick();
  btn3.tick();
}
void button_setup(){
  pinMode(RAIN_PIN,INPUT);
  pinMode(LM1_PIN,INPUT_PULLUP);
  pinMode(LM2_PIN,INPUT_PULLUP);
  btn1.attachClick(btn1_on_click);
  btn2.attachClick(btn2_on_click);
  btn3.attachClick(btn3_on_click);

}
//////////////////////////////////////////Mqtt
void wifi_setup() {
  pinMode(WF_LED_PIN,OUTPUT);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int timer1 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(millis()-timer1>5000) break;
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Mac address: ");
  Serial.println(WiFi.macAddress());
}
//mqtt
StaticJsonDocument<250> docIn;
StaticJsonDocument<250> docOut;
String jsonDataSend;
String jsonDataRcv;
uint32_t mqtt_now;

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
void firebase_setup(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}
void firebase_getData(){
  // bool tmp;
  // if(Firebase.getBool(ledData,FIREBASE_REM)){
  // tmp=ledData.boolData();
  // //
  // if(tmp=true){
  //   on_btn1;
  // }else on_btn2;
  //   Serial.println("fb 1");  
  // }
}
void firebase_loop(){
        if(Firebase.ready()){
        if(fbPub==false){
        firebase_getData();
        delay(2000); 
        } else{
            if(fbPub){
              if(Firebase.setBool(ledData,FIREBASE_REM,isOpen)) {fbPub=false; Serial.println("s1 ok");}
              else Serial.println("fb send 1 false");
          }  
        }
      }
}
/// mqtt
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
      client.subscribe(MQTT_REM_SUB);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void mqtt_callback(char* topic, byte *payload, unsigned int length){
  char buff[length];
    for(uint8_t i=0;i<length;i++){
      buff[i]=(char) payload[i];
    }
    buff[length]=0;
    docIn.clear();
    jsonDataRcv= String(buff);
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println( jsonDataRcv);
    DeserializationError err =deserializeJson(docIn,jsonDataRcv);
      if(!err){
        uint8_t tmp=docIn["MODE"];
        docIn.clear();
          if(strcmp(topic,MQTT_REM_SUB)==0){
            Serial.println(tmp);
           if(notRain){
            switch (tmp)
            {
            case 1:
              on_btn1();
              break;
            case 0:
              on_btn2();
              break;
            case 2:
              on_btn3();
              break;
            default:
              break;
            }
            }
          }
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
void mqtt_pub_ev(){
  if(client.connected()){
  if(mqttPub){
    mqtt_public(MQTT_REM_PUB,isOpen); mqttPub=false;
    }
  }
}
// FREERTOS
//
// void taskFirebaseSend(void * parameter) {
//   while (true) 
// }
//task
/////////////////////////
TaskHandle_t taskHandle_1;
TaskHandle_t taskHandle_2;  
TaskHandle_t taskHandle_3;  
// void taskSmartConfig(void *parameter){
//   (void) parameter;
// }
void taskFirebase(void * parameter) {
    (void) parameter;
  while (true) {
      
    if (WiFi.status()==WL_CONNECTED){
      // digitalWrite(WF_LED_PIN,HIGH);
      // if(Firebase.ready()){
      //   if(fbPub==false){
      //     firebase_getData();
            vTaskDelay(1500); 
           // Serial.println("task get1");
      //   } 
  //}
  }else{
    Serial.println("task get 2");
      WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
      digitalWrite(WF_LED_PIN,LOW);
      vTaskDelay(2000); 
   }
}
}

void taskFirebaseSend(void * parameter) {
  (void) parameter;
  
  while(1){
 if(Firebase.ready()){
 if(fbPub==true){
   Serial.println("task send 3");
   vTaskSuspend(taskHandle_1);
           if(fbPub){
              if(Firebase.setBool(ledData,FIREBASE_REM,isOpen)) {fbPub=false; Serial.println("s1 ok");}
              else Serial.println("fb send 1 false");
          }
       } else vTaskResume(taskHandle_1);
    }
  }
}
////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  relay_setup();
  button_setup();
  relay_to_stop();
  dc=TO_PAUSE;
  wifi_setup();
  mqtt_setup();
  firebase_setup();
  xTaskCreatePinnedToCore(taskFirebase, "TaskFirebase", 1024*15, NULL, 1,&taskHandle_1, 1);
  xTaskCreatePinnedToCore(taskFirebaseSend, "TaskFirebaseSend", 1024*15, NULL, 1, &taskHandle_2, 1);
}

void loop() {
  notRain=digitalRead(RAIN_PIN);
  if(WiFi.status()==WL_CONNECTED){
  mqtt_loop();
  mqtt_pub_ev();}
 if(notRain){
    button_loop();
   
    if(dc==TO_LEFT){
      if(digitalRead(LM1_PIN)==LOW){
        dc=TO_PAUSE;
        Serial.println("pause_open");
        isOpen=true;
        relay_to_stop();
        mqttPub=true;
        fbPub=true;
      }
    }
    if(dc==TO_RIGHT){
      if(digitalRead(LM2_PIN)==LOW){
        dc=TO_PAUSE;
        Serial.println("pause_closed");
        isOpen=false;
        relay_to_stop();
        mqttPub=true;
        fbPub=true;
      }
    }
    

  }else{
    if(isOpen){
      relay_to_right();
      dc=TO_RIGHT;
      if(digitalRead(LM2_PIN)==LOW){
        relay_to_stop();
        isOpen=false;
        dc=TO_PAUSE;
        Serial.println("pause_closed");
        mqttPub=true;
        fbPub=true;
     }
    }
  }
}