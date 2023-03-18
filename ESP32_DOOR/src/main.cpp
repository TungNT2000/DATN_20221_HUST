#include <Arduino.h>
#include<EEPROM.h>
#include<M_RFID.h>
#include<Keypad.h>
// #include<M_KEYPAD.h>
#include<M_DEFINE.h>
#include<FirebaseESP32.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include<ArduinoJson.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include<OneButton.h>
#include<Adafruit_MPR121.h>
/// //////////
void buz_bip(uint32_t t);
void buz_bip_n(uint32_t t, uint8_t n);
void state_suscess();
void state_failed();
#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] ={13,12,14,27};
byte pin_column[COLUMN_NUM] ={26,25,33}; 
// Adafruit_MPR121 cap = Adafruit_MPR121();
// char keypad_getkey(){
//     static uint16_t lasttouched = 0;
//     uint16_t currtouched = 0;
//     if (!cap.begin(0x5A)){
//       //  Serial.println("no touch");
//         return (char) 0;
//     }
//     currtouched = cap.touched();
//     uint8_t key=12;
//     for (uint8_t i=0; i<12; i++){
//         if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
//         key=i;
//         break;
//         }
//     }
//     lasttouched = currtouched;
//     switch(key)
//     {
//         case 0:
//         return '*';
//         case 1:
//         return '7';
//         case 2:
//         return '4';
//         case 3:
//         return '1';
//         case 4:
//         return '0';
//         case 5:
//         return '8';
//         case 6:
//         return '5';
//         case 7:
//         return '2';
//         case 8:
//         return '#';
//         case 9:
//         return '9';
//         case 10:
//         return '6';
//         case 11:
//         return '3';
//         default:
//         return (char)0;
//     }
// }
bool startEnter=false;
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
char password[PASS_LENGTH_MAX];
uint8_t keyCnt;
void keypad_setup(){
    Serial.println();
  //  cap.begin(0x5A);
    for(int i=0;i<11;i++){
        Serial.print("PASS=");
        Serial.print(EEPROM.read(PASS_STORAGE_ADDR+i));
    }
   
}
void keypad_append_pass(char key){
    //if(keyCnt<PASS_LENGTH_MAX){
    password[keyCnt]=key;
    keyCnt++;
   // }
}
void keypad_clear_pass(){
    for(uint8_t i=0;i<keyCnt;i++){
        password[i]=0;
    }
    keyCnt=0;
}
bool keypad_read_password(){
    static uint64_t time_now;
    static bool start =false;
    char key=keypad.getKey();
    //char key=keypad_getkey();
    if(key){
      buz_bip(100);
      time_now=millis();
        switch (key)
        {
        case '*':
        {
            Serial.println("bat dau nhap");
            keypad_clear_pass();
            start =true;
            break;
        }
        case '#':
           if(start){
            Serial.println("hoan thanh nhap");
            if(keyCnt>0)
            {start=false;
            return true;}
           }
           
        default:
            if(start) keypad_append_pass(key);
            break;
        }
        //test
        Serial.print("\npass= ");
        for(int i=0;i<keyCnt;i++){
            Serial.print(password[i]);
        }
    }
    if(start && (millis()-time_now>5000)){
      Serial.println("time out");
      start=false;
      keypad_clear_pass();
      return false;
    }
    return false;
}

bool keypad_is_password(char pass[]){
    // neu chua co pass-> xac nhan
    char *epass;
    uint8_t cnt=EEPROM.read(PASS_STORAGE_ADDR);
    if(keyCnt!=cnt) return false;
    for(uint8_t i=0;i<cnt;i++){
        if(pass[i]!=(char)EEPROM.read(PASS_STORAGE_ADDR+1+i)) return false;
    }
    return true; 
}
bool keypad_delete_pass(){
    if(EEPROM.read(PASS_STORAGE_ADDR)!=255){
        EEPROM.write(50,255);
        EEPROM.commit();
       // return true;
    }
   // return false;
   return true;
    
}
bool keypad_write_password(char pass[],uint8_t cnt){
    if(cnt==0) return false;
    EEPROM.write(PASS_STORAGE_ADDR,cnt);
    for(uint8_t i=0;i<keyCnt;i++){
        EEPROM.write(PASS_STORAGE_ADDR+i+1,pass[i]);
    }
    EEPROM.commit();
    //keypad_clear_pass();
    return true;
}
void keypad_coppy_arr(char arr1[],char  arr2[2],uint8_t size){
    for(uint8_t i=0;i<size;i++){
        arr1[i]= arr2[i];
    }
}
bool keypad_comp_arr(char arr1[],char arr2[],uint8_t size){
    for(uint8_t i=0;i<size;i++){
        if(arr1[i]!= arr2[i]) return false;
    } 
    return true;
}
char *keypad_get_pass(){
    return password;
}

/////////////////
bool isOpen=false;
bool isSettig=false;
bool buzerOn=true;
setting_mode_t settingMode=NONE;
uint64_t doorNow;
void buzer_on(uint32_t t,uint8_t cnt){
  if(buzer_on){
    uint32_t tmp= (uint32_t) t/cnt;
    for(uint8_t i=0;i<cnt;i++){
        if(i%2==0)  digitalWrite(BUZER_PIN,HIGH);
        else  digitalWrite(BUZER_PIN,LOW);
        delay(tmp);
    }
 }
}
void buz_bip(uint32_t t){
  digitalWrite(BUZER_PIN,HIGH);
  delay(t);
  digitalWrite(BUZER_PIN,LOW);
 // delay(100);
}
void buz_bip_n(uint32_t t, uint8_t n){
  for(uint8_t i=0;i<n;i++){
    if(i%2==0) digitalWrite(BUZER_PIN,HIGH);
    else digitalWrite(BUZER_PIN,LOW);
    delay(t);
  }
  digitalWrite(BUZER_PIN,LOW);
}
void state_suscess(){
  Serial.println("SUSCESS");
  digitalWrite(SUSCCESS_LED_PIN,HIGH);
  digitalWrite(FAILED_LED_PIN,LOW);
  buz_bip(100);
  digitalWrite(SUSCCESS_LED_PIN,LOW);
}
void state_failed(){
  Serial.println("FAILED");
  digitalWrite(SUSCCESS_LED_PIN,LOW);
  digitalWrite(FAILED_LED_PIN,HIGH);
  buz_bip(500);
  digitalWrite(FAILED_LED_PIN,LOW);
}
////////////////////mqtt
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
  digitalWrite(WF_LED_PIN,LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(millis()-timer1>5000) break;
  }
  if(WiFi.status() == WL_CONNECTED) digitalWrite(WF_LED_PIN,HIGH);
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
bool fbPub=false;
bool mqttPub=false;
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
OneButton btnOpen(BTN_OPEN_PIN,false,false);
//OneButton btnRst(BTN_RESET_PIN,true);
//OneButton btnRst(BTN_RESET_PIN,true);
void btnOpen_on_click(){
  Serial.println("btn_ok");
  buz_bip(100);
  if(!isOpen){
    isOpen=true;
    mqttPub=true;
    fbPub=true;
    digitalWrite(LOCK_PIN,HIGH);
  }
    doorNow=millis();
}
void btnRst_on_click(){
  Serial.println("reset");
}
void button_loop(){
  btnOpen.tick();
 // btnRst.tick();

 // btnRst.tick();
}
void button_setup(){
  btnOpen.attachClick(btnOpen_on_click);
 // btnRst.attachClick(btnRst_on_click);
}

/// firebase
void firebase_setup(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  now=millis();
}
void firebase_getData(){
   bool tmp;
  if(Firebase.getBool(ledData,FIREBASE_DOOR)){
  tmp=ledData.boolData();
  if(tmp!=isOpen){
    isOpen=tmp;
    if(isOpen) {digitalWrite(LOCK_PIN,HIGH);doorNow=millis();}
    else digitalWrite(LOCK_PIN,LOW);
    mqttPub=true;
    Serial.println("fb 1");
  }
  }
}
void firebase_loop(){
  if(Firebase.ready()){
    if(fbPub==false){
      firebase_getData();
      delay(2000); 
      } else{
          if(Firebase.setBool(ledData,FIREBASE_DOOR,isOpen)) {fbPub=false; Serial.println("s1 ok");}
          else Serial.println("fb send 1 false");
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

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe(MQTT_DOOR_STATE_SUB);
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
        bool set =docIn["SET"];
        uint8_t setMode=docIn["MODE"];
       
        docIn.clear();
          if(strcmp(topic,MQTT_DOOR_STATE_SUB)==0){
             doorNow=millis();
             if(tmp!=isOpen){
              fbPub=true;
              isOpen=tmp;
              if(isOpen=true) {digitalWrite(LOCK_PIN,HIGH);   }
              else digitalWrite(LOCK_PIN,LOW);
              }
              if(set){ 
                isSettig=true; digitalWrite(LOCK_PIN,HIGH);
                settingMode=(setting_mode_t)setMode;
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
void mqtt_public(char*Topic,bool open,bool set, uint8_t mode){
    jsonDataSend="";
    docOut.clear();
    docOut["ON"]=open;
    docOut["SET"]=set;
    docOut["MODE"]=mode;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
  client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void mqtt_pub_ev(){
  if(client.connected()){
    if(mqttPub){
      mqtt_public(MQTT_DOOR_STATE_PUB,isOpen,isSettig,settingMode); mqttPub=false;
    }
  }
}
/////////////////////////
TaskHandle_t taskHandle_1;
TaskHandle_t taskHandle_2;  
void taskFirebase(void * parameter) {
    (void) parameter;
  while (true) {
      
    if (WiFi.status()==WL_CONNECTED){
    //  mqtt_loop();
      digitalWrite(WF_LED_PIN,HIGH);
      if(Firebase.ready()){
        if(fbPub==false){
          firebase_getData();
          vTaskDelay(1500); 
          // Serial.println("task get1");
        } 
  }
  }else{
     // Serial.println("task get 2");
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
    if(Firebase.setBool(ledData,FIREBASE_DOOR,isOpen)) {fbPub=false; Serial.println("s1 ok");}
    else Serial.println("fb send 1 false");
       } else vTaskResume(taskHandle_1);
    }
  }
}
void gpio_setup(){
  pinMode(WF_LED_PIN,OUTPUT);
  pinMode(BUZER_PIN,OUTPUT);
  pinMode(LOCK_PIN,OUTPUT);
  pinMode(SETTING_LED_PIN,OUTPUT);
  pinMode(SUSCCESS_LED_PIN,OUTPUT);
  pinMode(FAILED_LED_PIN,OUTPUT);
}

////////////////////
void setup() { 
  Serial.begin(115200);
  gpio_setup();
  button_setup();
  wifi_setup();
  mqtt_setup();
  EEPROM.begin(100);
  rfid_setup();
  //cap.begin(0x5A);
 // keypad_setup();
 firebase_setup();
  xTaskCreatePinnedToCore(taskFirebase, "TaskFirebase", 1024*15, NULL, 1,&taskHandle_1, 1);
  xTaskCreatePinnedToCore(taskFirebaseSend, "TaskFirebaseSend", 1024*15, NULL, 1, &taskHandle_2, 1);  
}


void loop() {
  mqtt_loop();
  uint8_t *UID;
  if(WiFi.status()==WL_CONNECTED){
  button_loop();
  mqtt_pub_ev();}
  digitalWrite(SETTING_LED_PIN,isSettig);
  digitalWrite(LOCK_PIN,isOpen);

  if(isSettig){
   // digitalWrite(LOCK_PIN,HIGH);
    switch (settingMode)
    {
    case NONE:
    {
      char key = keypad.getKey();
      //char key =keypad_getkey();
      if(key){
        buz_bip(100);
        Serial.println(key);
        doorNow=millis();
        switch (key)
        {
            case '1': 
                Serial.println("add card");
                settingMode=ADD_CARD;
                mqttPub=true;
                break;
            case '2': 
                Serial.println("dell card");
                settingMode=DEL_CARD;
                mqttPub=true;
                break;
            case '3': 
                Serial.println("change master");
                settingMode=CHANGE_MASTER;
                mqttPub=true;
                break;
            case '4': 
                Serial.println("change pass");
                settingMode=CHANGE_PASS;
                mqttPub=true;
                break;
            case '5': 
                Serial.println("dell all member");
                settingMode=DELETE_ALL_CARD;
                mqttPub=true;
                break;
            case '#': 
                Serial.println("exit");
                buz_bip(500);
                isOpen=false;
                isSettig=false;
                mqttPub=true;
                fbPub=true;
                break;
        }
      }
        break;
    }
    case ADD_CARD:
    {
      if(rfid_read_card()){
  //    //   buz_bip(100);
        doorNow=millis();
        UID=rfid_get_uid_card();
        if(!rfid_uid_is_master(UID)&&rfid_write_member_uid(UID)) state_suscess();
        else state_failed();
        settingMode=NONE;
        mqttPub=true;
      }
      break;
    }
    case DEL_CARD:
    {
      if(rfid_read_card()){
  //    //   buz_bip(100);
        doorNow=millis();
        UID=rfid_get_uid_card();
        if( rfid_delete_member_uid(UID)){
         state_suscess();
        }else state_failed();
        settingMode=NONE;
        mqttPub=true;
      }
      break;
    }
    case CHANGE_MASTER:
    {
      if(rfid_read_card()){
        doorNow=millis();
        UID=rfid_get_uid_card();
        if(rfid_uid_is_master(UID)||rfid_uid_is_storage(UID)){
          state_failed();
        }else {rfid_write_master_uid(UID); Serial.println("ok");state_suscess();}
         settingMode=NONE;
         mqttPub=true;
      }
      break;
    }
    case DELETE_ALL_CARD:
    {
      if(rfid_read_card()){
        doorNow=millis();
        UID=rfid_get_uid_card();
        if(!rfid_uid_is_master(UID)){
          state_failed();
        }else if(rfid_delete_all_member_uid()) {Serial.println("ok");state_suscess();}
         settingMode=NONE;
         mqttPub=true;
      }
      break;
    }
    case CHANGE_PASS:
    {
      char key2=keypad.getKey();
     // char key2=keypad_getkey();
      if(key2){
           buz_bip(100);
        doorNow=millis();
        switch (key2)
          {
          case '*':
          { 
              startEnter=true;
              Serial.println("bat dau nhap");
              keypad_clear_pass();
              break;
          }
          case '#':
            if(startEnter){
              startEnter=false;
              Serial.println("hoan thanh nhap");
              if(keyCnt>0)
                if(keypad_write_password(password,keyCnt)){
                  state_suscess();
                  keypad_clear_pass();
                }
            } else{
              state_failed();
              settingMode=NONE;
              mqttPub=true;
              doorNow=millis();
            }
          default:
          // check do dai{}
              if(startEnter){
              keypad_append_pass(key2);
              Serial.print("\npass= ");
              for(int i=0;i<keyCnt;i++){
              Serial.print(password[i]);}
              break;
              }
          }
     }
     // check timer out
     if(millis()-doorNow>5000){
      startEnter=false;
      Serial.println("time out");
      keypad_clear_pass();
      settingMode=NONE;
      mqttPub=true;
      doorNow=millis();
     }
     
      // }
      break;
    }
    default:
      break;
    }
    if(millis()-doorNow>10000) {
      buz_bip(500);
      isSettig = false;
      isOpen   = false;
      keypad_clear_pass();
      settingMode=NONE;
      Serial.println("close");
      fbPub=true;
      mqttPub  = true;      
      }  
  } else{
    if(isOpen){
    digitalWrite(LOCK_PIN,HIGH);
    if(rfid_read_card()){
      UID=rfid_get_uid_card();
       if(rfid_uid_is_master(UID)) {buz_bip_n(100,3);isSettig=true, doorNow=millis(); Serial.println("Setting");mqttPub=true;}}
    if(millis()-doorNow>10000){
      Serial.println("close");
      buz_bip(500);
      isOpen=false, mqttPub=true; fbPub=true;digitalWrite(LOCK_PIN,LOW);}
  }else{
    if(rfid_read_card()){
     // doorNow=millis();
      digitalWrite(LOCK_PIN,LOW);
      Serial.println("read ok");
      UID = rfid_get_uid_card();
      if(rfid_uid_is_master(UID)) {buz_bip_n(100,5);isSettig=true; doorNow=millis();isOpen=true;mqttPub=true;fbPub=true;}
      else if(rfid_uid_is_storage(UID)) {state_suscess();doorNow=millis(); Serial.println("member");isOpen=true; mqttPub=true; fbPub=true;}
      else{ Serial.println("erro");state_failed();}
      }
       if(keypad_read_password()){
         if(keypad_is_password(password)){state_suscess();doorNow=millis(); Serial.println("member");isOpen=true; mqttPub=true; fbPub=true;}
         else { Serial.println("erro");state_failed();}
       }
    }
  }
}
/////////






//  }
  // if(rfid_read_card()){
  //   Serial.println("read ok");
  //   UID = rfid_get_uid_card();
  //   if(rfid_uid_is_master(UID)) Serial.println("master");
  //   else if (rfid_uid_is_storage (UID)) Serial.println("member");
  //   else Serial.println("none");
  // }
//  CHECK RFID
//    uint8_t *uid;
//   if(Serial.available()>0){
//     c=Serial.read();
//     Serial.println(c);
//     now=millis();
//     switch (c)
//     {
//     case 'A':
//     if(mode!=mode_open){
//         mode=mode_open;
//         digitalWrite(LOOK_PIN,HIGH);
//         Serial.println("door is open");
//        // buzer_on(500,1);
//         now=millis();}
//       break;
//     case 'B':
//       if(mode!= mode_close){
//        // buzer_on(1000,2);
//         mode=mode_close;
//         Serial.println("door is closed");}
//       break;
//     case 'C':
//     if(mode!=mode_setting){
//         mode=mode_setting;
//         digitalWrite(LOOK_PIN,HIGH);
//         //buzer_on(1500,3);
//         Serial.println("door is Setting");
//         now=millis();}
//       break;
//     default:
//       break;
//     }
//     }

//   // }
//   // if(rfid_read_card()){
//   //   now=millis();
//   //   Serial.println("\nread ok");
//   //   uid=rfid_get_uid_card();
//   //   for(int i=0;i<4;i++){
//   //     Serial.println(uid[i],HEX);
//   //   }
//   //   switch (c)
//   //   {
//   //   case 'A':
//   //   Serial.println("write_member");
//   //   t=rfid_write_member_uid(uid);
//   //     break;
//   //   case 'B':
//   //   Serial.println("dell member");
//   //   t=rfid_delete_member_uid(uid);
//   //     break;
//   //   case 'C':
//   //   Serial.println("write master");
//   //   t=rfid_write_master_uid(uid);
//   //     break;
//   //   case 'D':
//   //    Serial.println("dell master");
//   //   t=rfid_delete_master_uid();
//   //     break;
//   //   case 'E':
//   //   Serial.println("dell all");
//   //   t=rfid_delete_all_member_uid();
//   //     break;
//   //   case 'F':
//   //    Serial.println("is storage");
//   //   t=rfid_uid_is_storage(uid);
//   //     break;
//   //   case 'H':
//   //    Serial.println("is master");
//   //   t=rfid_uid_is_master(uid);
//   //     break;
//   //   default:
//   //    // c='';
//   //     break;
//   //   }
//   //   if(t) {
//   //     Serial.println("ok");
//   //     buzer_on(1000);
//   //     }
//   //   else {Serial.println("false");buzer_on(3000);}
   
//   // }
//   //  if(millis()-now>5000){
//   //     Serial.println("exitMode");
//   //       c='o';
//   //   }
// //  if(doorIsSetting){
// //   if(millis)


//  }
 
//  if(doorIsOpen=false){
//     if(rfid_read_card()){
//       now=millis();
//       Serial.println("\nread ok");
//       uid=rfid_get_uid_card();
//       for(int i=0;i<4;i++){
//         Serial.println(uid[i],HEX);
//     }
//     if(rfid_uid_is_storage(uid)){
//       Serial.println("door is Opened");
//       buzer_on(1000);
//       now=millis();
//       doorIsOpen=true;
//       digitalWrite(LOOK_PIN,HIGH);
//     }
//  } else{
//     if(millis()-now >5000){
//       Serial.println("door is close");
//       doorIsOpen=false;
//       digitalWrite(LOOK_PIN,LOW);
//       buzer_on(1000);
//     }
//  }

// }

// switch (mode)
// {
//   case mode_open:
//     if(millis()-now>5000){
//       Serial.println("door is closed");
//       digitalWrite(LOOK_PIN,LOW);
//      // buzer_on(500,2);
//       mode=mode_close;
//     } else{
//         if(rfid_read_card()){
//             // now=millis();
//         Serial.println("\nread ok");
//         uid=rfid_get_uid_card();
//         for(int i=0;i<4;i++){
//             Serial.println(uid[i],HEX);
//             }
//         if(rfid_uid_is_storage(uid)){
//             Serial.println("door is Opened");
//             //buzer_on(500,1);
//             now=millis();
//             mode=mode_open;
//             digitalWrite(LOOK_PIN,HIGH);
//         }else if(rfid_uid_is_master(uid)){
//             Serial.println("door is setting");
//            // buzer_on(500,3);
//             now=millis();
//             mode=mode_setting;
//             digitalWrite(LOOK_PIN,HIGH);
//           }else // buzer_on(3000,1);
//         }
//       }
//     break;
//   case mode_close:
//     if(rfid_read_card()){
//           // now=millis();
//       Serial.println("\nread ok");
//       uid=rfid_get_uid_card();
//       for(int i=0;i<4;i++){
//           Serial.println(uid[i],HEX);
//           }
//       if(rfid_uid_is_storage(uid)){
//           Serial.println("door is Opened");
//         //  buzer_on(500,1);
//           now=millis();
//           mode=mode_open;
//           digitalWrite(LOOK_PIN,HIGH);
//       }else if(rfid_uid_is_master(uid)){
//           Serial.println("door is Opened");
//         //  buzer_on(500,3);
//           now=millis();
//           mode=mode_setting;
//           digitalWrite(LOOK_PIN,HIGH);
//         }else // buzer_on(3000,1);
//       }
//     break;
//   case mode_setting:
//   if(millis()-now>5000){
//       Serial.println("door is closed");
//       digitalWrite(LOOK_PIN,LOW);
//      // buzer_on(500,2);
//       mode=mode_close;
//     } else{
//       digitalWrite(LOOK_PIN,HIGH);
        // if(rfid_read_card()){
        //     // now=millis();
        // Serial.println("\nread ok");
        // uid=rfid_get_uid_card();
        // for(int i=0;i<4;i++){
        //     Serial.println(uid[i],HEX);
        //     }
        // if(rfid_uid_is_storage(uid)){
        //     Serial.println("door is Opened");
        //     buzer_on(500,1);
        //     now=millis();
        //     mode=mode_open;
        //     digitalWrite(LOOK_PIN,HIGH);
        // }else if(rfid_uid_is_master(uid)){
        //     Serial.println("door is setting");
        //     buzer_on(500,3);
        //     now=millis();
        //     mode=mode_setting;
        //     digitalWrite(LOOK_PIN,HIGH);
        //   }else  buzer_on(3000,1);
        // }
 //     }
    
  //   break;
  // default:
  //   break;
//}
//}