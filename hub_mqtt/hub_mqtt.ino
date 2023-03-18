#include <Arduino.h>
#include"M_DEFINE.h"
#include <RMaker.h>
#include <WiFi.h>
#include <WiFiProv.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneButton.h>
OneButton btnRst(BTN_RST_PIN,true);
void btn_rst_on_click(){
  Serial.println("rst");
  RMakerWiFiReset(5);
}
void button_setup(){
   btnRst.attachClick(btn_rst_on_click);
}
void button_loop(){
  btnRst.tick();
}
// mqtt
StaticJsonDocument<250> docIn;
StaticJsonDocument<250> docOut;
String jsonDataSend;
String jsonDataRcv;
uint32_t mqtt_now;
bool buzOn=false;
// RMaker
const char *service_name = "PROV_2023";
const char *pop = "datn_2023";
bool sw1On,sw2On,sw3On,sw4On,sw5On,sw6On;
static Switch sw1,sw2,sw3,sw4,sw5,sw6,buz;
static Device dm1("Dimmer1", "custom.device.dimmer");
static Device dm2("Dimmer2", "custom.device.dimmer");
static Device dm3("Dimmer3", "custom.device.dimmer");

uint8_t ledDm1,ledDm2,ledDm3;
bool ledOn1,ledOn2,ledOn3;
static Device door("DOOR","custom.device.device");
const char* options[] = {"NONE", "ADD MEMBER", "DELETE MEMBER", "CHANGE MASTER","CHANGE PASSWORD","DELETE ALL MEMBER"};
bool doorIsOpen=false;
bool doorIsSetting=false;
uint8_t doorSetMode=0;
char *settingStr="NONE";
static Device cur("Curtains", "custom.device.curtains");
bool curState;
bool cur1, cur2,cur3;
// rain
static Device rain("Rain", "custom.device.curtains");
bool rainState;
bool rain1, rain2,rain3;
//string doorSettingMode='';// 0 none, 1
/// cur
//DHT
static TemperatureSensor temperature("Temperature");
static TemperatureSensor humidity("Humidity");
float temValue, humValue;
//PIR
bool pir1On,pir2On,pir3On;
static Switch pir1,pir2,pir3;
//
// static TemperatureSensor GAS("GAS");
// float gasValue;
bool gasOn;
static Switch gas;
//
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int mqtt_interval=5000;
// function
void sysProvEvent(arduino_event_t *sys_event);
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx);
void rmaker_setup();
// mqtt
void mqtt_setup();
void mqtt_connect_to_broker();
void mqtt_callback(char* topic, byte *payload, unsigned int length);
void mqtt_loop();
void mqtt_public(char* pubTopic,bool rl);
//
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
      // ON/OFF 1
      client.subscribe(MQTT_FO_B1_RL1);
      client.subscribe(MQTT_FO_B1_RL2);
      client.subscribe(MQTT_FO_B1_RL3);
      //ON/OFF 2
      client.subscribe(MQTT_FO_B2_RL1);
      client.subscribe(MQTT_FO_B2_RL2);
      client.subscribe(MQTT_FO_B2_RL3);
      //DM 1
      client.subscribe(MQTT_FO_B2_DM1);
      client.subscribe(MQTT_FO_B2_DM2);
      client.subscribe(MQTT_FO_B2_DM3);
      // DOOR
      client.subscribe(MQTT_FO_DOOR_STATE);
      //REM
      client.subscribe(MQTT_FO_REM);
      //RAIN
      client.subscribe(MQTT_FO_RAIN);
      // SENSOR
      client.subscribe(MQTT_FO_DHT_HUM);
      client.subscribe(MQTT_FO_DHT_TEM);
      client.subscribe(MQTT_FO_GAS);
      client.subscribe(MQTT_FO_PIR1);
      client.subscribe(MQTT_FO_PIR2);
      //client.subscribe(MQTT_FO_PIR3);


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
    jsonDataRcv= String(buff);
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println( jsonDataRcv);
    DeserializationError err =deserializeJson(docIn,jsonDataRcv);
  // Switch on the LED if an 1 was received as first character
      if(!err){
          if(strcmp(topic,MQTT_FO_B1_RL1)==0){
               sw1On= docIn["ON"];
               docIn.clear();
               sw1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw1On);
              ////////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B1_RL2)==0){
                sw2On= docIn["ON"];
                 docIn.clear();
               sw2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw2On);
               //////////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B1_RL3)==0){
               sw3On=docIn["ON"];
               docIn.clear();
               sw3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw3On);
               //////////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B2_RL1)==0){
               sw4On= docIn["ON"];
               docIn.clear();
               sw4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw4On);
              ////////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B2_RL2)==0){
                sw5On= docIn["ON"];
                docIn.clear();
                sw5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw5On);
               //////////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B2_RL3)==0){
               sw6On=docIn["ON"];
               docIn.clear();
               sw6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,sw6On);
               //////////////////PIR
          }else if(strcmp(topic,MQTT_FO_PIR1)==0){
               pir1On=docIn["ON"];
               docIn.clear();
               pir1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,pir1On);
          //////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_PIR2)==0){
               pir2On=docIn["ON"];
               docIn.clear();
               pir2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,pir2On);
          //////////////////////////////////
          // }else if(strcmp(topic,MQTT_FO_PIR3)==0){
          //      pir3On=docIn["ON"];
          //      docIn.clear();
          //      pir3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,pir3On);
          //////////////////////////////////
          }else if(strcmp(topic,MQTT_FO_B2_DM1)==0){
                ledOn1= docIn["ON"]; 
                ledDm1= docIn["VAL"];
                docIn.clear();
                dm1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,ledOn1);
                dm1.updateAndReportParam("Level",ledDm1);
          }else if(strcmp(topic,MQTT_FO_B2_DM2)==0){
                    ledOn2= docIn["ON"]; 
                    ledDm2= docIn["VAL"];
                    docIn.clear();
                    dm2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,ledOn2);
                    dm2.updateAndReportParam("Level",ledDm2);
          }else if(strcmp(topic,MQTT_FO_B2_DM3)==0){
                    ledOn3= docIn["ON"]; 
                    ledDm3= docIn["VAL"];
                    docIn.clear();
                    dm3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,ledOn3);
                    dm3.updateAndReportParam("Level",ledDm3); 
                    /////////////////  
          }else if(strcmp(topic,MQTT_FO_GAS)==0){
                gasOn= docIn["ON"];
                docIn.clear();                    
                gas.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,gasOn);
                //////////////////
          }else if(strcmp(topic,MQTT_FO_DHT_TEM)==0){
                temValue=docIn["VAL"];
                docIn.clear();                           
                temperature.updateAndReportParam("Temperature", temValue);
                ///////////////
          }else if(strcmp(topic,MQTT_FO_DHT_HUM)==0){
                humValue=docIn["VAL"];
                docIn.clear();                           
                humidity.updateAndReportParam("Temperature", humValue);                                             
          }else if(strcmp(topic,MQTT_FO_DOOR_STATE)==0){
                   doorIsOpen=docIn["ON"];
                   doorIsSetting=docIn["SET"];
                   doorSetMode=docIn["MODE"];
                   docIn.clear();
                   door.updateAndReportParam("Door open",doorIsOpen);
                   door.updateAndReportParam("Setting",doorIsSetting);
                   door.updateAndReportParam("Setting options",options[doorSetMode]);
          }else if(strcmp(topic,MQTT_FO_REM)==0){
                   curState=docIn["ON"];
                   docIn.clear();
                   if(curState) cur.updateAndReportParam("display","OPENED");
                   else cur.updateAndReportParam("display","CLOSED");
          }else if(strcmp(topic,MQTT_FO_RAIN)==0){
                   rainState=docIn["ON"];
                   docIn.clear();
                   if(rainState) rain.updateAndReportParam("display","OPENED");
                   else rain.updateAndReportParam("display","CLOSED");
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
void mqtt_public_dm(char*Topic,bool rl,uint8_t i){
    jsonDataSend="";
    docOut.clear();
    docOut["ON"]=rl;
    docOut["VAL"]=i;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void mqtt_public_door(char *Topic,bool state, bool set, uint8_t mode){
    jsonDataSend="";
    docOut.clear();
    docOut["ON"]=state;
    docOut["SET"]=set;
    docOut["MODE"]=mode;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
void mqtt_public_cur(char *Topic,uint8_t num){
    jsonDataSend="";
    docOut.clear();
    docOut["MODE"]=num;
    serializeJson(docOut,jsonDataSend);
    Serial.println("pub");
    Serial.println(jsonDataSend);
    // send mqtt // bool
    client.publish(Topic,(uint8_t *)jsonDataSend.c_str(), jsonDataSend.length());
}
// RMaker
// function
void sysProvEvent(arduino_event_t *sys_event);
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx);
void rmaker_setup();
/// 
/// 
void sysProvEvent(arduino_event_t *sys_event){
    switch (sys_event->event_id) {
          case ARDUINO_EVENT_PROV_START:
  #if CONFIG_IDF_TARGET_ESP32S2
              Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
              printQR(service_name, pop, "softap");
  #else
              Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
              printQR(service_name, pop, "ble");
  #endif
              break;
          case ARDUINO_EVENT_PROV_INIT:
              wifi_prov_mgr_disable_auto_stop(10000);
              break;
          case ARDUINO_EVENT_PROV_CRED_SUCCESS:
              wifi_prov_mgr_stop_provisioning();
              break;
          default:;
      }
}
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx){
  const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();
    if(strcmp(device_name,"sw1")==0) {
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          sw1On = val.val.b;
          mqtt_public(MQTT_TO_B1_RL1,sw1On);
          param->updateAndReport(val);}
          /////////////////////////////////
    }else if(strcmp(device_name,"sw2")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          sw2On = val.val.b;
          mqtt_public(MQTT_TO_B1_RL2,sw2On);
          param->updateAndReport(val);}
          ////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"sw3")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          sw3On = val.val.b;
          mqtt_public(MQTT_TO_B1_RL3,sw3On);
          param->updateAndReport(val);}
          //////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"sw4")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          sw4On = val.val.b;
          mqtt_public(MQTT_TO_B2_RL1,sw4On);
          param->updateAndReport(val);}
          //////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"sw5")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          sw5On = val.val.b;
          mqtt_public(MQTT_TO_B2_RL2,sw5On);
          param->updateAndReport(val);}
          //////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"sw6")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          sw6On = val.val.b;
          mqtt_public(MQTT_TO_B2_RL3,sw6On);
          param->updateAndReport(val);}
          //////////////////////////////////////////////////////////////////PIR
    }else if(strcmp(device_name,"BUZZER")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
          buzOn = val.val.b;
         // mqtt_public(MQTT_TO_B2_RL3,sw6On);
          param->updateAndReport(val);}
    }else if(strcmp(device_name,"PIR1")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
        //  pir1On = val.val.b;
        //   mqtt_public(MQTT_TO_PIR1,pir1On);
          //param->updateAndReport(val);}
          pir1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,pir1On);}
          //////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"PIR2")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
        //  pir1On = val.val.b;
        //   mqtt_public(MQTT_TO_PIR1,pir1On);
          //param->updateAndReport(val);}
          pir2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,pir2On);}
          ////////////////////////////////////////////////////////////////// GAS
    }else if(strcmp(device_name,"GAS")==0){
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          // get status
        //  pir1On = val.val.b;
        //   mqtt_public(MQTT_TO_PIR1,pir1On);
          //param->updateAndReport(val);}
          pir3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,gasOn);}
          //////////////////////////////////////////////////////////////////
    } else if(strcmp(device_name,"Dimmer1")==0){
      if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        ledOn1 = val.val.b;
        mqtt_public_dm(MQTT_TO_B2_DM1, ledOn1,ledDm1);
        param->updateAndReport(val);
    } else if (strcmp(param_name, "Level") == 0) {
        Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
        ledDm1=val.val.i;
        if(ledOn1) mqtt_public_dm(MQTT_TO_B2_DM1, ledOn1,ledDm1);
        param->updateAndReport(val);}
        /////////////////////////////////////////////////////////////////////////
    } else if(strcmp(device_name,"Dimmer2")==0){
      if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        ledOn2 = val.val.b;
        mqtt_public_dm(MQTT_TO_B2_DM2, ledOn2,ledDm2);
        param->updateAndReport(val);
    } else if (strcmp(param_name, "Level") == 0) {
        Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
        ledDm2=val.val.i;        
          if(ledOn2) mqtt_public_dm(MQTT_TO_B2_DM2, ledOn2,ledDm2);
        param->updateAndReport(val);}
        //////////////////////////////////////////////////////////////////////////////
    } else if(strcmp(device_name,"Dimmer3")==0){
      if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
        ledOn3 = val.val.b;
        mqtt_public_dm(MQTT_TO_B2_DM3, ledOn3,ledDm3);
        param->updateAndReport(val);
    } else if (strcmp(param_name, "Level") == 0) {
        Serial.printf("\nReceived value = %d for %s - %s\n", val.val.i, device_name, param_name);
         ledDm3=val.val.i;
        if(ledOn3)  mqtt_public_dm(MQTT_TO_B2_DM3, ledOn3,ledDm3);
        param->updateAndReport(val);}
        ///////////////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"DOOR")==0){
      if(strcmp(param_name, "Door open") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
           doorIsOpen=val.val.b;
           if(!doorIsOpen) {doorIsSetting=false; doorSetMode=0; 
           door.updateAndReportParam("Setting",doorIsSetting);
           door.updateAndReportParam("Setting options",options[doorSetMode]);
           }
           mqtt_public_door(MQTT_TO_DOOR_STATE,doorIsOpen,doorIsSetting,doorSetMode);
           param->updateAndReport(val);
      } else if(strcmp(param_name, "Setting options") == 0) {
          Serial.printf("\nReceived value = %s for %s - %s\n",val.val.s, device_name, param_name);
          settingStr=val.val.s;
          for(uint8_t i=0;i<6;i++){
            if(strcmp(options[i],settingStr)==0)
            {doorSetMode=i;break;}
          } 
          if(doorIsSetting){
            mqtt_public_door(MQTT_TO_DOOR_STATE,doorIsOpen,doorIsSetting,doorSetMode);
          }
          param->updateAndReport(val);
      }else if(strcmp(param_name, "Setting") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          doorIsSetting=val.val.b;
          if(doorIsSetting){
            doorIsOpen=true;
            door.updateAndReportParam("Door open", doorIsOpen);
          } else { door.updateAndReportParam("Setting options", options[0]); doorSetMode=0;}
          mqtt_public_door(MQTT_TO_DOOR_STATE,doorIsOpen,doorIsSetting,doorSetMode);
          param->updateAndReport(val);}
          /////////////////////////////////////////////////////////////////////////////
    }else if(strcmp(device_name,"Curtains")==0){
      if(strcmp(param_name, "Left") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
           cur1=val.val.b;
           if(cur1)  mqtt_public_cur(MQTT_TO_REM,1);
           param->updateAndReport(val);
      } else if(strcmp(param_name,"Pause") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
           cur2=val.val.b;
           if(cur2)  mqtt_public_cur(MQTT_TO_REM,0);
           param->updateAndReport(val);
      }else if(strcmp(param_name, "Right") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          cur3=val.val.b;
           if(cur3)  mqtt_public_cur(MQTT_TO_REM,2);
          param->updateAndReport(val);}
          //////////////////////////rain
    }else if(strcmp(device_name,"Rain")==0){
      if(strcmp(param_name, "Left") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
           rain1=val.val.b;
           if(rain1)  mqtt_public_cur(MQTT_TO_RAIN,1);
           param->updateAndReport(val);
      } else if(strcmp(param_name,"Pause") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
           rain2=val.val.b;
           if(rain2)  mqtt_public_cur(MQTT_TO_RAIN,0);
           param->updateAndReport(val);
      }else if(strcmp(param_name, "Right") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
          rain3=val.val.b;
           if(rain3)  mqtt_public_cur(MQTT_TO_RAIN,2);
          param->updateAndReport(val);}
    }
      //changeState=true;
}
void rmaker_setup(){
    Node my_node;
    my_node = RMaker.initNode("RMAKER HUB");

    //Initialize switch device
    //b1
    sw1 = Switch("sw1");
    sw2 = Switch("sw2");
    sw3 = Switch("sw3");
    sw1.addCb(write_callback);
    sw2.addCb(write_callback);
    sw3.addCb(write_callback);
    my_node.addDevice(sw1);
    my_node.addDevice(sw2);
    my_node.addDevice(sw3);
    
    //b2
    sw4 = Switch("sw4");
    sw5 = Switch("sw5");
    sw6 = Switch("sw6");
    //
    sw4.addCb(write_callback);
    sw5.addCb(write_callback);
    sw6.addCb(write_callback);
    //
    my_node.addDevice(sw4);
    my_node.addDevice(sw5);
    my_node.addDevice(sw6);
  // buz
    buz = Switch("BUZZER");
    buz.addCb(write_callback);
    my_node.addDevice(buz);

    // dm
    dm1.addNameParam();
    dm1.addPowerParam(DEFAULT_POWER_MODE);
    dm1.assignPrimaryParam(dm1.getParamByName(ESP_RMAKER_DEF_POWER_NAME));
    Param level1_param("Level", "custom.param.level", value(DEFAULT_DIMMER_LEVEL), PROP_FLAG_READ | PROP_FLAG_WRITE);
    level1_param.addBounds(value(0), value(100), value(1));
    level1_param.addUIType(ESP_RMAKER_UI_SLIDER);
    dm1.addParam(level1_param);
    dm1.addCb(write_callback);
    my_node.addDevice(dm1);
    //
    dm2.addNameParam();
    dm2.addPowerParam(DEFAULT_POWER_MODE);
    dm2.assignPrimaryParam(dm2.getParamByName(ESP_RMAKER_DEF_POWER_NAME));
    Param level2_param("Level", "custom.param.level", value(DEFAULT_DIMMER_LEVEL), PROP_FLAG_READ | PROP_FLAG_WRITE);
    level2_param.addBounds(value(0), value(100), value(1));
    level2_param.addUIType(ESP_RMAKER_UI_SLIDER);
    dm2.addParam(level2_param);
    dm2.addCb(write_callback);
    my_node.addDevice(dm2);
    ////
    dm3.addNameParam();
    dm3.addPowerParam(DEFAULT_POWER_MODE);
    dm3.assignPrimaryParam(dm3.getParamByName(ESP_RMAKER_DEF_POWER_NAME));
    Param level3_param("Level", "custom.param.level", value(DEFAULT_DIMMER_LEVEL), PROP_FLAG_READ | PROP_FLAG_WRITE);
    level3_param.addBounds(value(0), value(100), value(1));
    level3_param.addUIType(ESP_RMAKER_UI_SLIDER);
    dm3.addParam(level3_param);
    dm3.addCb(write_callback);
    my_node.addDevice(dm3);
    //Standard switch device
  //Add custom dimmer device to the node
    //gas
    gas = Switch("GAS");
    gas.addCb(write_callback);
    my_node.addDevice(gas);
    // dht gas
   // my_node.addDevice(GAS);
    my_node.addDevice(temperature);
    my_node.addDevice(humidity);
    //pir
    pir1 = Switch("PIR1");
    pir1.addCb(write_callback);
    my_node.addDevice(pir1);
    //pir2
    pir2 = Switch("PIR2");
    pir2.addCb(write_callback);
    my_node.addDevice(pir2);
    //pir 3
    // pir3 = Switch("PIR3");
    // pir3.addCb(write_callback);
    // my_node.addDevice(pir3);
        //////////door
    door.addNameParam();
    Param disp("display","custom.param.display",value("HUST_DOOR"),PROP_FLAG_READ);
    disp.addUIType(ESP_RMAKER_UI_TEXT);
    door.addParam(disp);
    Param open_sw("Door open","custom.param.power",value(doorIsOpen),PROP_FLAG_READ|PROP_FLAG_WRITE);
    open_sw.addUIType(ESP_RMAKER_UI_TOGGLE);
    door.addParam(open_sw);
    Param setting_sw("Setting","custom.param.power",value(doorIsSetting),PROP_FLAG_READ|PROP_FLAG_WRITE);
    setting_sw.addUIType(ESP_RMAKER_UI_TOGGLE);
    door.addParam(setting_sw);
    Param setting_drop("Setting options","custom.param.dropdown",value(options[0]),PROP_FLAG_READ|PROP_FLAG_WRITE);
    setting_drop.addValidStrList(options,1);
    setting_drop.addValidStrList(options,2);
    setting_drop.addValidStrList(options,3);
    setting_drop.addValidStrList(options,4);
    setting_drop.addValidStrList(options,5);
    setting_drop.addValidStrList(options,6);
    setting_drop.addUIType(ESP_RMAKER_UI_DROPDOWN); // chọn kiểu UI là dropdown
    door.addParam(setting_drop);
    // Param buz_sw("Buzzer","custom.param.power",value(doorOnBuzer),PROP_FLAG_READ|PROP_FLAG_WRITE);
    // open_sw.addUIType(ESP_RMAKER_UI_TOGGLE);
    // door.addParam(buz_sw);
    door.addCb(write_callback);
    my_node.addDevice(door);
    ////////////cur
    cur.addNameParam();
    Param disp_cur("display","custom.param.display",value("CLOSED"),PROP_FLAG_READ);
    disp_cur.addUIType(ESP_RMAKER_UI_TEXT);
    cur.addParam(disp_cur);
    RMaker.enableOTA(OTA_USING_TOPICS);
    Param left("Left","custom.param.trigger",value(false),PROP_FLAG_WRITE);
    left.addUIType(ESP_RMAKER_UI_TRIGGER);
    cur.addParam(left);
    Param pause("Pause","custom.param.trigger",value(true),PROP_FLAG_WRITE);
    pause.addUIType(ESP_RMAKER_UI_TRIGGER);
    cur.addParam(pause);
    Param right("Right","custom.param.trigger",value(false),PROP_FLAG_WRITE);
    right.addUIType(ESP_RMAKER_UI_TRIGGER);
    cur.addParam(right);
    cur.addCb(write_callback);
    my_node.addDevice(cur);

    // rain
    rain.addNameParam();
    Param disp_rain("display","custom.param.display",value("CLOSED"),PROP_FLAG_READ);
    disp_rain.addUIType(ESP_RMAKER_UI_TEXT);
    rain.addParam(disp_rain);
    RMaker.enableOTA(OTA_USING_TOPICS);
    Param left_rain("Left","custom.param.trigger",value(false),PROP_FLAG_WRITE);
    left_rain.addUIType(ESP_RMAKER_UI_TRIGGER);
    rain.addParam(left_rain);
    Param pause_rain("Pause","custom.param.trigger",value(true),PROP_FLAG_WRITE);
    pause_rain.addUIType(ESP_RMAKER_UI_TRIGGER);
    rain.addParam(pause_rain);
    Param right_rain("Right","custom.param.trigger",value(false),PROP_FLAG_WRITE);
    right_rain.addUIType(ESP_RMAKER_UI_TRIGGER);
    rain.addParam(right_rain);
    rain.addCb(write_callback);
    my_node.addDevice(rain);
    RMaker.enableOTA(OTA_USING_TOPICS);
    //
    
    //
    //If you want to enable scheduling, set time zone for your region using setTimeZone().
    //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
    // RMaker.setTimeZone("Asia/Shanghai");
    // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
    RMaker.enableTZService();

    RMaker.enableSchedule();

    RMaker.enableScenes();

    RMaker.start();

    WiFi.onEvent(sysProvEvent);

    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
}
void buzzer_on(bool on){
  if(on){
    digitalWrite(BUZZER_PIN,HIGH);
  }else digitalWrite(BUZZER_PIN,LOW);
}
void setup() {
  Serial.begin(115200);
  rmaker_setup();
  button_setup();
  mqtt_setup();
  pinMode(BUZZER_PIN,OUTPUT);
    

}
bool buzF=false;
void loop() {
  button_loop();
  buzzer_on(buzOn);
  mqtt_loop();
  if(buzOn&& buzF==false){
    esp_rmaker_raise_alert("canh bao ngoi nha dang co van de");
    buzF=true;
  } else if(!buzOn && buzF==true){
    buzF=false;
  }
}
