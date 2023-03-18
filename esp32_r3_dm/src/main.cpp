#include <M_DEFINE.h>
#include<WiFi.h>
#include <FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <OneButton.h>

//flag
void wifi_setup() {
  pinMode(WF_LED_PIN,OUTPUT);
  delay(10);
  // We start by connecting to a WiFi network
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
bool mqttPub1=false,mqttPub2=false,mqttPub3=false;
bool fbPubStt1=false,fbPubStt2=false,fbPubStt3=false;
bool fbPubVal1=false,fbPubVal2=false,fbPubVal3=false;
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
uint8_t led1Dm=80,led2Dm=80,led3Dm=80;
void btn1_on_click(){
  Serial.println("btn1_ok");
  led1On=toggle_state(led1On);
  if(led1On){
    led1_set_dimmer(led1Dm);
  } else led1_set_dimmer(0);
   mqttPub1=true;
   fbPubStt1=true;
   fbPubVal1=true;
}
void btn2_on_click(){
  Serial.println("btn2_ok");
  led2On=toggle_state(led2On);
  if(led2On){
    led2_set_dimmer(led2Dm);
  } else led2_set_dimmer(0);
   mqttPub2=true;
   fbPubStt2=true;
   fbPubVal2=true;
}
void btn3_on_click(){
  Serial.println("btn3_ok");
  led3On=toggle_state(led3On);
  if(led3On){
    led3_set_dimmer(led3Dm);
  } else led3_set_dimmer(0);
   mqttPub3=true;
   fbPubStt3=true;
   fbPubVal3=true;
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
  bool btmp;
  uint8_t itmp;
 if( Firebase.getBool(ledData,FIREBASE_DM1_STT)){
  btmp=ledData.boolData();
  if(btmp!=led1On){
    led1On=btmp;
    if(!led1On){
      led1_set_dimmer(0);
    } else{
      Firebase.getInt(ledData,FIREBASE_DM1_VAL);
      led1Dm=ledData.intData();
      led1_set_dimmer(led1Dm);
    }
    mqttPub1=true;
  } else{
    if(led1On) {
      Firebase.getInt(ledData,FIREBASE_DM1_VAL);
      itmp=ledData.intData();
      if(itmp!=led1Dm){
        led1Dm=itmp;
          led1_set_dimmer(led1Dm);
           mqttPub1=true;
        }
      }
  }
 }
 ledData.clear();
  if(Firebase.getBool(ledData,FIREBASE_DM2_STT)){
  btmp=ledData.boolData();
  if(btmp!=led2On){
    led2On=btmp;
    if(!led2On){
      led2_set_dimmer(0);
    } else{
      Firebase.getInt(ledData,FIREBASE_DM2_VAL);
      led2Dm=ledData.intData();
      led2_set_dimmer(led2Dm);
    }
    mqttPub2=true;
  } else{
    if(led2On) {
      Firebase.getInt(ledData,FIREBASE_DM2_VAL);
      itmp=ledData.intData();
      if(itmp!=led2Dm){
        led2Dm=itmp;
          led2_set_dimmer(led2Dm);
           mqttPub2=true;
        }
      }
  }
}
 ledData.clear();
  if(Firebase.getBool(ledData,FIREBASE_DM3_STT)){
  btmp=ledData.boolData();
  if(btmp!=led3On){
    led3On=btmp;
    if(!led3On){
      led3_set_dimmer(0);
    } else{
      Firebase.getInt(ledData,FIREBASE_DM3_VAL);
      led3Dm=ledData.intData();
      led3_set_dimmer(led3Dm);
    }
    mqttPub3=true;
  } else{
    if(led3On) {
      Firebase.getInt(ledData,FIREBASE_DM3_VAL);
      itmp=ledData.intData();
      if(itmp!=led3Dm){
        led3Dm=itmp;
          led3_set_dimmer(led3Dm);
           mqttPub3=true;
        }
      }
  }
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
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(MQTT_DM1_SUB);
      client.subscribe(MQTT_DM2_SUB);
      client.subscribe(MQTT_DM3_SUB);
    }else {
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
        bool tmp=docIn["ON"];
        uint8_t val=docIn["VAL"];
        Serial.println(val);
        docIn.clear();
          if(strcmp(topic,MQTT_DM1_SUB)==0){
             if(tmp==false){
                led1_set_dimmer(0);
                led1On=false;
                fbPubStt1=true;
              }else{
                led1On=true;
                led1Dm=val;
                led1_set_dimmer(led1Dm);
                fbPubStt1=true;
                fbPubVal1=true;
              }
          }else if(strcmp(topic,MQTT_DM2_SUB)==0){
              if(tmp==false){
                led2_set_dimmer(0);
                led2On=false;
                fbPubStt2=true;
              }else{
                led2On=true;
                led2Dm=val;
                led2_set_dimmer(led2Dm);
                fbPubStt2=true;
                fbPubVal2=true;
              }
           }else if(strcmp(topic,MQTT_DM3_SUB)==0){
             if(tmp==false){
                led3_set_dimmer(0);
                led3On=false;
                fbPubStt3=true;
              }else{
                led3On=true;
                led3Dm=val;
                led3_set_dimmer(led3Dm);
                fbPubStt3=true;
                fbPubVal3=true;
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
    docOut.clear();
    docOut["ON"]=s;
    docOut["VAL"]=val;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void mqtt_pub_ev(){
  if(client.connected()){
  if(mqttPub1){
    mqtt_public(MQTT_DM1_PUB,led1On,led1Dm); mqttPub1=false;
    }
  if(mqttPub2){
    mqtt_public(MQTT_DM2_PUB,led2On,led2Dm); mqttPub2=false;
    }
  if(mqttPub3){
    mqtt_public(MQTT_DM3_PUB,led3On,led3Dm); mqttPub3=false;
    }
  }
}

TaskHandle_t taskHandle_1;
TaskHandle_t taskHandle_2;  
void taskFirebase(void * parameter) {
    (void) parameter;
  while (true) {
      
    if (WiFi.status()==WL_CONNECTED){
      digitalWrite(WF_LED_PIN,HIGH);
      if(Firebase.ready()){
        if(fbPubStt1==false && fbPubStt2==false && fbPubStt3==false &&fbPubVal1==false && fbPubVal2==false && fbPubVal3==false ){
          firebase_getData();
          vTaskDelay(2000); 
           Serial.println("task get1");
      } 
  }
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
 if(fbPubStt1==true || fbPubStt2==true || fbPubStt3==true ||fbPubVal1==true || fbPubVal2==true || fbPubVal3==true ){
   Serial.println("task send 3");
   vTaskSuspend(taskHandle_1);
    ledData.clear();
          if(fbPubStt1){
              if(Firebase.setBool(ledData,FIREBASE_DM1_STT,led1On)) {fbPubStt1=false; Serial.println("s1 stt ok");}
              else Serial.println("fb send 1 false");
          }else if(fbPubVal1){
              if(Firebase.setInt(ledData,FIREBASE_DM1_VAL,led1Dm)) {fbPubVal1=false; Serial.println("s1 val ok");}
              else Serial.println("fb send 1 false");
          }else if(fbPubStt2){
              if(Firebase.setBool(ledData,FIREBASE_DM2_STT,led2On)) {fbPubStt2=false; Serial.println("s2 stt ok");}
              else Serial.println("fb send 2 false");
          } else if(fbPubVal2){
              if(Firebase.setInt(ledData,FIREBASE_DM2_VAL,led2Dm)) {fbPubVal2=false; Serial.println("s2 val ok");}
              else Serial.println("fb send 2 false");
          }
          if(fbPubStt3){
              if(Firebase.setBool(ledData,FIREBASE_DM3_STT,led3On)) {fbPubStt3=false; Serial.println("s3 stt ok");}
              else Serial.println("fb send 3 false");
          }else if(fbPubVal3){
              if(Firebase.setInt(ledData,FIREBASE_DM3_VAL,led3Dm)) {fbPubVal3=false; Serial.println("s3 val ok");}
              else Serial.println("fb send 3 false");
          }
       } else vTaskResume(taskHandle_1);
  }
  }
}
void setup() {
   Serial.begin(115200);
  button_setup();
  wifi_setup();
  mqtt_setup();
  led_setup();
  firebase_setup();
  xTaskCreatePinnedToCore(taskFirebase, "TaskFirebase", 1024*15, NULL, 1,&taskHandle_1, 1);
  xTaskCreatePinnedToCore(taskFirebaseSend, "TaskFirebaseSend", 1024*15, NULL, 1, &taskHandle_2, 1);
  mqttPub1=true;
  mqttPub2=true;
  mqttPub3=true;
}

void loop() {
  button_loop();
  if(WiFi.status()==WL_CONNECTED){
  mqtt_pub_ev();
  mqtt_loop();}
 // firebase_loop();

}