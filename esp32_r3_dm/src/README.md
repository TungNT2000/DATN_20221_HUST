#include <M_DEFINE.h>
#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <OneButton.h>
#include <M_WIFI.h>
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
void mqtt_public(char* pubTopic,bool s, uint8_t val);
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
bool led1On,led2On,led3On;
uint8_t led1Dm=100,led2Dm=100,led3Dm=100;
void btn1_on_click(){
  Serial.println("btn1_ok");
  led1On=toggle_state(led1On);
  if(led1On){
    led_set_dimmer(LED1_PIN,led1Dm);
  } else led_set_dimmer(LED1_PIN,0);
  //pub
   mqtt_public(MQTT_DM1_PUB,led1On,led1Dm);
   Firebase.setBool(ledData,FIREBASE_DM1_STT,led1On);
   Firebase.setBool(ledData,FIREBASE_DM1_VAL,led1Dm);
}
void btn2_on_click(){
  Serial.println("btn2_ok");
  led2On=toggle_state(led2On);
   if(led2On){
    led_set_dimmer(LED2_PIN,led2Dm);
  } else led_set_dimmer(LED2_PIN,0);
  //
     mqtt_public(MQTT_DM1_PUB,led2On,led2Dm);
     Firebase.setBool(ledData,FIREBASE_DM1_STT,led2On);
     Firebase.setBool(ledData,FIREBASE_DM1_VAL,led2Dm);
}
void btn3_on_click(){
  Serial.println("btn3_ok");
  led3On=toggle_state(led3On);
   if(led1On){
  led_set_dimmer(LED3_PIN,led3Dm);
  } else led_set_dimmer(LED3_PIN,0);
    mqtt_public(MQTT_DM1_PUB,led3On,led3Dm);
   Firebase.setBool(ledData,FIREBASE_DM1_STT,led3On);
   Firebase.setBool(ledData,FIREBASE_DM1_VAL,led3Dm); 
}
void btn1_on_long_press(){
  if(!led1On) return;
  led1Dm= uint8_t(led1Dm/10)*10;
  if(led1Dm==100) led1Dm=0;
  else led1Dm=led1Dm+10;
}
void btn1_after_long_press(){
  if(!led1On) return;
  mqtt_public(MQTT_DM1_PUB,led1On,led1Dm);
  Firebase.setBool(ledData,FIREBASE_DM1_VAL,led1Dm);
}
void button_loop(){
  btn1.tick();
  btn2.tick();
  btn3.tick();
 // btnRst.tick();
}
void button_setup(){
  btn1.attachClick(btn1_on_click);
  btn1.attachDuringLongPress(btn1_on_long_press);
  btn1.attachLongPressStop(btn1_on_long_press);
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
   bool btmp;
  Firebase.getBool(ledData,FIREBASE_DM1_STT);
  btmp=ledData.boolData();
  if(btmp!=led1On){
    led1On=btmp;
    if(!led1On) led_set_dimmer(LED1_PIN,0);
    else{
      Firebase.getInt(ledData,FIREBASE_DM1_VAL);
      led1Dm=ledData.intData();
      led_set_dimmer(LED1_PIN,led1Dm);
    }
    mqtt_public(MQTT_DM1_PUB,led1On,led1Dm);
  }
 // Firebase.getBool(ledData,FIREBASE_RL2);
//   tmp=ledData.boolData();
//   if(tmp!=rl2On){
//     rl2On=tmp;
//     digitalWrite(RL2_PIN,rl2On);
//    // mqtt_public(MQTT_RL2_PUB,rl2On);
//   }
//  // Firebase.getBool(ledData,FIREBASE_RL3);
//   tmp=ledData.boolData();
//   if(tmp!=rl3On){
//     rl3On=tmp;
//     digitalWrite(RL3_PIN,rl3On);
//    // mqtt_public(MQTT_RL1_PUB,rl3On);
//   }
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
      client.subscribe(MQTT_DM1_SUB);
      client.subscribe(MQTT_DM2_SUB);
      client.subscribe(MQTT_DM3_SUB);
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
void mqtt_public(char*Topic,bool s, uint8_t val){
    jsonDataSend="";
    docOut["ON"]=s;
    docOut["VAL"]=val;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
//
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