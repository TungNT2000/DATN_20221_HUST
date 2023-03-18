
#include <WiFi.h>
#include<M_WIFI.h>
#include<M_DEFINE.h>
// ssid va pass wifi EEPROM

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
void wifi_loop(){
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(WF_LED_PIN,HIGH);
  }else digitalWrite(WF_LED_PIN,LOW);
}
void wifi_get_mac_addr(){
  WiFi.macAddress();
}
void wifi_get_ip_addr(){
  WiFi.localIP();
}
