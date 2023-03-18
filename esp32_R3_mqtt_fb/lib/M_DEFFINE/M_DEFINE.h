#include<Arduino.h>
#define FIREBASE_HOST "smart-home-36c8d-default-rtdb.asia-southeast1.firebasedatabase.app" //Without http:// or https:// schemes
#define FIREBASE_AUTH "AIzaSyBXQO0Xt7h8DK2UgNIHYcl4chV6RWgcypI"
#define LENGTH(x) (strlen(x) + 1)   // length of char string
#define EEPROM_SIZE 100    
// #define WIFI_SSID "FPT Telecom 5G"
// #define WIFI_PASSWORD "123456777"
#define WIFI_SSID "Win10"
#define WIFI_PASSWORD "00000000"
// in
#define BTN1_PIN 15
#define BTN2_PIN 16
#define BTN3_PIN 17
#define BTN_RESET_PIN 21
#define WF_LED_PIN 23
//out
#define RL1_PIN 12
#define RL2_PIN 14
#define RL3_PIN 27

// mqtt server
//#define MQTT_SERVER "192.168.1.90"// server
#define MQTT_SERVER "192.168.43.91"// server
#define MQTT_PORT 1883//port
// sub
#define MQTT_RL1_SUB "board1_rl1_in"
#define MQTT_RL2_SUB "board1_rl2_in"
#define MQTT_RL3_SUB "board1_rl3_in"
//pub
#define MQTT_RL1_PUB "board1_rl1_out"
#define MQTT_RL2_PUB "board1_rl2_out"
#define MQTT_RL3_PUB "board1_rl3_out"
// fb
#define FIREBASE_RL1 "/Smart_home/livingroom/fan_status"
#define FIREBASE_RL2 "/Smart_home/bathrom/lamp_status"
#define FIREBASE_RL3 "/Smart_home/readingrom/air_status"

void relay_setup();

