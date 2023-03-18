#include<M_DEFINE.h>
#include<FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include<OneButton.h>
#include<M_WIFI.h>
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
// bool fbChangeState;// flag for firebase
// bool mqttChangeState;// flag for mqtt
uint64_t now;
bool toggle_state(bool s){
  return !s;
}
// btn
OneButton btn1(BTN1_PIN,false,false);
OneButton btn2(BTN2_PIN,false,false);
OneButton btn3(BTN3_PIN,false,false);
//OneButton btnRst(BTN_RESET_PIN,true);
bool rl1On=false,rl2On=false,rl3On=false;
void btn1_on_click(){
  Serial.println("btn1_ok");
  rl1On=toggle_state(rl1On);
  digitalWrite(RL1_PIN,rl1On); 
  mqtt_public(MQTT_RL1_PUB,rl1On);
  Firebase.setBool(ledData,FIREBASE_RL1,rl1On);
}
void btn2_on_click(){
  Serial.println("btn2_ok");
  rl2On=toggle_state(rl2On);
  digitalWrite(RL2_PIN,rl2On); 
  mqtt_public(MQTT_RL2_PUB,rl2On);
  Firebase.setBool(ledData,FIREBASE_RL2,rl2On); 
}
void btn3_on_click(){
  Serial.println("btn3_ok");
  rl3On=toggle_state(rl3On);
  digitalWrite(RL3_PIN,rl3On);
  mqtt_public(MQTT_RL3_PUB,rl3On); 
  Firebase.setBool(ledData,FIREBASE_RL3,rl3On);   
}
void button_loop(){
  btn1.tick();
  btn2.tick();
  btn3.tick();
 // btnRst.tick();
}
void button_setup(){
  btn1.attachClick(btn1_on_click);
  btn2.attachClick(btn2_on_click);
  btn3.attachClick(btn3_on_click);

}

/// firebase
void firebase_setup(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  now=millis();
}
void firebase_getData(){
   bool tmp;
  Firebase.getBool(ledData,FIREBASE_RL1);
  tmp=ledData.boolData();
  if(tmp!=rl1On){
    rl1On=tmp;
    digitalWrite(RL1_PIN,rl1On);
    mqtt_public(MQTT_RL1_PUB,rl1On);
  }
  Firebase.getBool(ledData,FIREBASE_RL2);
  tmp=ledData.boolData();
  if(tmp!=rl2On){
    rl2On=tmp;
    digitalWrite(RL2_PIN,rl2On);
    mqtt_public(MQTT_RL2_PUB,rl2On);
  }
  Firebase.getBool(ledData,FIREBASE_RL3);
  tmp=ledData.boolData();
  if(tmp!=rl3On){
    rl3On=tmp;
    digitalWrite(RL3_PIN,rl3On);
    mqtt_public(MQTT_RL1_PUB,rl3On);
  }
}
void firebase_loop(){
  if(millis()-now>1000){
    firebase_getData();
    now=millis();
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
        bool tmp;
          if(strcmp(topic,MQTT_RL1_SUB)==0){
             tmp=docIn["ON"];
             if(tmp!=rl1On){
              rl1On=tmp;
              digitalWrite(RL1_PIN,rl1On);
              Firebase.setBool(ledData,FIREBASE_RL1,rl1On);
             }
          }else if(strcmp(topic,MQTT_RL2_SUB)==0){
              tmp=docIn["ON"];
              if(tmp!=rl2On){
              rl2On=tmp;
              digitalWrite(RL2_PIN,rl2On);
              Firebase.setBool(ledData,FIREBASE_RL2,rl2On);
             }
           }else if(strcmp(topic,MQTT_RL3_SUB)==0){
              tmp=docIn["ON"];
              if(tmp!=rl3On){
              rl3On=tmp;
              digitalWrite(RL3_PIN,rl3On);
              Firebase.setBool(ledData,FIREBASE_RL3,rl3On);
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
    docOut["ON"]=rl;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
// FREERTOS

void setup() {
  Serial.begin(115200);
  button_setup();
  wifi_setup();
  mqtt_setup();
  relay_setup();
  firebase_setup();
}

void loop() {
  button_loop();
  wifi_loop();
  mqtt_loop();
  firebase_loop();

}