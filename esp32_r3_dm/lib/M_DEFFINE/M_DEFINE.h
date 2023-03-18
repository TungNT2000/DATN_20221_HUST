#include<Arduino.h>
#define FIREBASE_HOST "smart-home-36c8d-default-rtdb.asia-southeast1.firebasedatabase.app" //Without http:// or https:// schemes
#define FIREBASE_AUTH "AIzaSyBXQO0Xt7h8DK2UgNIHYcl4chV6RWgcypI"
// #define WIFI_SSID "FPT Telecom 5G"
// #define WIFI_PASSWORD "123456777"
#define WIFI_SSID "Win10"
#define WIFI_PASSWORD "00000000"
// #define WIFI_SSID "FPT Telecom 5G"
// #define WIFI_PASSWORD "123456777"
// in
#define BTN1_PIN 15
#define BTN2_PIN 16
#define BTN3_PIN 17
#define BTN_RESET_PIN 21
//dm
#define WF_LED_PIN 23
#define FREQ 5000 
#define LED1_CHANNEL 0
#define LED2_CHANNEL 1
#define LED3_CHANNEL 2
#define RESOLUTION 8

#define DEFAULT_POWER_MODE true
#define DEFAULT_DIMMER_LEVEL 100

#define LED1_PIN 12
#define LED2_PIN 14
#define LED3_PIN 27

// mqtt server
//  #define MQTT_SERVER "192.168.1.90"// server
#define MQTT_SERVER "192.168.43.91"// server
 #define MQTT_PORT 1883//port

//#define MQTT_SERVER "07c362791e4446f69c23283ff80e9cc7.s1.eu.hivemq.cloud"// server
//#define MQTT_SERVER "192.168.43.91"// server
//#define MQTT_PORT 8883//port
#define MQTT_UNAME "smartHome_2023"
#define MQTT_PASSWORD "doAnTotNghiep2023"
// sub
#define MQTT_DM1_SUB "board2_dm1_in"
#define MQTT_DM2_SUB "board2_dm2_in"
#define MQTT_DM3_SUB "board2_dm3_in"
//pub
#define MQTT_DM1_PUB "board2_dm1_out"
#define MQTT_DM2_PUB "board2_dm2_out"
#define MQTT_DM3_PUB "board2_dm3_out"
// fb
// #define FIREBASE_DM1_STT "/Smart_home/bathroom/lamp_status"
// #define FIREBASE_DM1_VAL "/Smart_home/bathroom/lamp_value"
// #define FIREBASE_DM2_STT "/Smart_home/bedroom/air_status"
// #define FIREBASE_DM2_VAL "/Smart_home/bedroom/air_value"
#define FIREBASE_DM1_STT "/Smart_home/bedroom/lamp_status"
#define FIREBASE_DM1_VAL "/Smart_home/bedroom/lamp_value"
#define FIREBASE_DM2_STT "/Smart_home/readingrom/lamp_status"
#define FIREBASE_DM2_VAL "/Smart_home/readingrom/lamp_value"
#define FIREBASE_DM3_STT "/Smart_home/livingrom/lamp_status"
#define FIREBASE_DM3_VAL "/Smart_home/livingrom/lamp_value"


void led_setup();
 // xem lai
void led1_set_dimmer(uint8_t dutyCycle);
void led2_set_dimmer(uint8_t dutyCycle);
void led3_set_dimmer(uint8_t dutyCycle);