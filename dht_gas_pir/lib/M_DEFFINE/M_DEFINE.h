#include<Arduino.h>
#define FIREBASE_HOST "smart-home-36c8d-default-rtdb.asia-southeast1.firebasedatabase.app" //Without http:// or https:// schemes
#define FIREBASE_AUTH "AIzaSyBXQO0Xt7h8DK2UgNIHYcl4chV6RWgcypI"
// #define WIFI_SSID "FPT Telecom 5G"
// #define WIFI_PASSWORD "123456777"
#define WIFI_SSID "Win10"
#define WIFI_PASSWORD "00000000"
// in
// #define BTN1_PIN 15
// #define BTN2_PIN 16
// #define BTN3_PIN 17
// #define BTN_RESET_PIN 21
//out
#define PIR1_PIN 13
#define PIR2_PIN 26
#define PIR3_PIN 32
// GAS
#define GAS_PIN 34
#define DHT_PIN 5
#define DHTTYPE DHT11 
// mqtt server
// #define MQTT_SERVER "192.168.1.90"// server
 #define MQTT_SERVER "192.168.43.91"// server
#define MQTT_PORT 1883//port
// #define MQTT_SERVER "07c362791e4446f69c23283ff80e9cc7.s1.eu.hivemq.cloud"
// #define MQTT_PORT 8883
// sub
#define MQTT_GAS_SUB "gas_in"
#define MQTT_DHT_SUB "dht_in"
#define MQTT_RL1_SUB "board1_rl1_in"
#define MQTT_RL2_SUB "board1_rl2_in"
#define MQTT_RL3_SUB "board1_rl3_in"
//pub
#define MQTT_GAS_PUB "gas_out"
#define MQTT_DHT_TEM_PUB "dht_tem_out"
#define MQTT_DHT_HUM_PUB "dht_hum_out"
#define MQTT_PIR1_PUB "pir1_out"
#define MQTT_PIR2_PUB "pir2_out"
#define MQTT_PIR3_PUB "pir3_out"
///////////////
#define MQTT_RL1_PUB "board1_rl1_out"
#define MQTT_RL2_PUB "board1_rl2_out"
#define MQTT_RL3_PUB "board1_rl3_out"
// fb
#define FIREBASE_RL1 "/Smart_home/livingroom/fridge_status"
#define FIREBASE_RL2 "/Smart_home/livingroom/fan_status"
#define FIREBASE_RL3 "/Smart_home/livingroom/lamp_status"
#define FIREBASE_GAS "/Smart_home/kitchen/GAS"
#define FIREBASE_DHT "/Smart_home/bedroom/dht"


void relay_setup();
