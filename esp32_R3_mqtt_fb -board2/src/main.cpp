#include<M_DEFINE.h>
#include<FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include<OneButton.h>
// #include<M_WIFI.h>
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
//mqtt
StaticJsonDocument<250> docIn;
StaticJsonDocument<250> docOut;
String jsonDataSend;
String jsonDataRcv;
uint32_t mqtt_now;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int mqtt_interval=5000;
bool fbPub1F=false,fbPub2F=false,fbPub3F=false;
bool mqtt1PubF=false,mqtt2PubF=false,mqtt3PubF=false;
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
 // mqtt_public(MQTT_RL1_PUB,rl1On);
  mqtt1PubF=true;
  fbPub1F=true;
 // Firebase.setBool(ledData,FIREBASE_RL1,rl1On);
}
void btn2_on_click(){
  Serial.println("btn2_ok");
  rl2On=toggle_state(rl2On);
  digitalWrite(RL2_PIN,rl2On); 
 // mqtt_public(MQTT_RL2_PUB,rl2On);
  mqtt2PubF=true;
  fbPub2F=true;
  //Firebase.setBool(ledData,FIREBASE_RL2,rl2On); 
}
void btn3_on_click(){
  Serial.println("btn3_ok");
  rl3On=toggle_state(rl3On);
  digitalWrite(RL3_PIN,rl3On);
 // mqtt_public(MQTT_RL3_PUB,rl3On); 
    mqtt3PubF=true;
 // Firebase.setBool(ledData,FIREBASE_RL3,rl3On);   
   fbPub3F=true;
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
  if(Firebase.getBool(ledData,FIREBASE_RL1)){
  tmp=ledData.boolData();
  if(tmp!=rl1On){
    rl1On=tmp;
    digitalWrite(RL1_PIN,rl1On);
    mqtt1PubF=true;
    //mqtt_public(MQTT_RL1_PUB,rl1On);
    Serial.println("fb 1");
  }
  }
  if(Firebase.getBool(ledData,FIREBASE_RL2)){
  tmp=ledData.boolData();
  if(tmp!=rl2On){
    rl2On=tmp;
    Serial.println("fb 2");
    digitalWrite(RL2_PIN,rl2On);
   // mqttPub1F=true;
    mqtt2PubF=true;
   // mqtt_public(MQTT_RL2_PUB,rl2On);
  }
  }
  if(Firebase.getBool(ledData,FIREBASE_RL3)){
  tmp=ledData.boolData();
  if(tmp!=rl3On){
    rl3On=tmp;
    digitalWrite(RL3_PIN,rl3On);
   mqtt3PubF=true;
   Serial.println("fb 3");
    //mqtt_public(MQTT_RL1_PUB,rl3On);
  }
  }
}
void firebase_loop(){
        if(Firebase.ready()){
        if(fbPub1F==false && fbPub2F==false && fbPub3F==false){
      
        firebase_getData();
        delay(2000); 
        } else{
            if(fbPub1F){
              if(Firebase.setBool(ledData,FIREBASE_RL1,rl1On)) {fbPub1F=false; Serial.println("s1 ok");}
              else Serial.println("fb send 1 false");
          }
            if(fbPub2F){
              if(Firebase.setBool(ledData,FIREBASE_RL2,rl2On)) {fbPub2F=false; Serial.println("s2 ok");}
              else Serial.println("fb send 2 false");
          }
            if(fbPub3F){
              if(Firebase.setBool(ledData,FIREBASE_RL3,rl3On)) {fbPub3F=false; Serial.println("s3 ok");}
              else Serial.println("fb send 3 false");
          }
        }
      }
  //   firebase_getData();
  //   now=millis();
  // }
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
  // Switch on the LED if an 1 was received as first character
      if(!err){
        bool tmp=docIn["ON"];
        docIn.clear();
          if(strcmp(topic,MQTT_RL1_SUB)==0){
             if(tmp!=rl1On){
              rl1On=tmp;
              digitalWrite(RL1_PIN,rl1On);
              fbPub1F=true;
             }
          }else if(strcmp(topic,MQTT_RL2_SUB)==0){
              if(tmp!=rl2On){
              rl2On=tmp;
              digitalWrite(RL2_PIN,rl2On);
              fbPub2F=true;
             }
           }else if(strcmp(topic,MQTT_RL3_SUB)==0){
              if(tmp!=rl3On){
              rl3On=tmp;
              digitalWrite(RL3_PIN,rl3On);
              fbPub3F=true;
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
  if(mqtt1PubF){
    mqtt_public(MQTT_RL1_PUB,rl1On); mqtt1PubF=false;
  }
  if(mqtt2PubF){
    mqtt_public(MQTT_RL2_PUB,rl2On); mqtt2PubF=false;
  }
  if(mqtt3PubF){
    mqtt_public(MQTT_RL3_PUB,rl3On); mqtt3PubF=false;
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
      digitalWrite(WF_LED_PIN,HIGH);
      if(Firebase.ready()){
        if(fbPub1F==false && fbPub2F==false && fbPub3F==false){
          firebase_getData();
          vTaskDelay(1500); 
           Serial.println("task get1");
        } 
        
        // else{
        //     if(fbPub1F){
        //       if(Firebase.setBool(ledData,FIREBASE_RL1,rl1On)) {fbPub1F=false; Serial.println("s1 ok");}
        //       else Serial.println("fb send 1 false");
        //   }
        //     if(fbPub2F){
        //       if(Firebase.setBool(ledData,FIREBASE_RL2,rl2On)) {fbPub2F=false; Serial.println("s2 ok");}
        //       else Serial.println("fb send 2 false");
        //   }
        //     if(fbPub3F){
        //       if(Firebase.setBool(ledData,FIREBASE_RL3,rl3On)) {fbPub3F=false; Serial.println("s3 ok");}
        //       else Serial.println("fb send 3 false");
        //   }
        // }
     // }
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
 if(fbPub1F==true || fbPub2F==true || fbPub3F==true){
   Serial.println("task send 3");
   vTaskSuspend(taskHandle_1);
           if(fbPub1F){
              if(Firebase.setBool(ledData,FIREBASE_RL1,rl1On)) {fbPub1F=false; Serial.println("s1 ok");}
              else Serial.println("fb send 1 false");
          }
            if(fbPub2F){
              if(Firebase.setBool(ledData,FIREBASE_RL2,rl2On)) {fbPub2F=false; Serial.println("s2 ok");}
              else Serial.println("fb send 2 false");
          }
            if(fbPub3F){
              if(Firebase.setBool(ledData,FIREBASE_RL3,rl3On)) {fbPub3F=false; Serial.println("s3 ok");}
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
  relay_setup();
  firebase_setup();
  xTaskCreatePinnedToCore(taskFirebase, "TaskFirebase", 1024*15, NULL, 1,&taskHandle_1, 1);
  xTaskCreatePinnedToCore(taskFirebaseSend, "TaskFirebaseSend", 1024*15, NULL, 1, &taskHandle_2, 1);
  mqtt1PubF=true;
  mqtt2PubF=true;
  mqtt3PubF=true;
  //xTaskCreatePinnedToCore(taskSmartConfig, "TaskSmartConfig", 1024*15, NULL, 1, &taskHandle_3, 1);
  //
  // xTaskCreatePinnedToCore(taskFirebaseSend, "TaskFirebaseSend", 1024*5, NULL, 1,NULL, 1);
}

void loop() {
  button_loop();
  mqtt_pub_ev();
  mqtt_loop();
 // wifi_loop();
  
 // firebase_loop();

}