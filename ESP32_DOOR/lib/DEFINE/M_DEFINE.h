// #define BUZER_PIN 13
// #define LOOK_PIN 12
// typedef enum{
//     mode_close    =0,
//     mode_open     =1,
//     mode_setting  =2,
// }door_mode_t;
// typedef enum{
//     mode_setting_none=0,
//     mode_setting_add_Card=1,
//     mode_setting_remove_card=2,
//     mode_setting_dell_master=3,
//     mode_setting_change_pass=4,
// }door_setting_t;
#include<Arduino.h>
#define FIREBASE_HOST "smart-home-36c8d-default-rtdb.asia-southeast1.firebasedatabase.app" //Without http:// or https:// schemes
#define FIREBASE_AUTH "AIzaSyBXQO0Xt7h8DK2UgNIHYcl4chV6RWgcypI"
// #define WIFI_SSID "FPT Telecom 5G"
// #define WIFI_PASSWORD "123456777"
#define WIFI_SSID "Win10"
#define WIFI_PASSWORD "00000000"
// in
#define BUZER_PIN 32//ok
#define LOCK_PIN 15//ok
#define BTN_OPEN_PIN 34//???
#define SETTING_LED_PIN 17//ok
#define SUSCCESS_LED_PIN 16//ok
#define FAILED_LED_PIN  2//0k
 #define BTN_RESET_PIN 22//ok
#define WF_LED_PIN 5

// #define BUZER_PIN 25
// #define LOCK_PIN 13
// #define BTN_OPEN_PIN 16
// #define SETTING_LED_PIN 14
// #define SUSCCESS_LED_PIN 27
// #define FAILED_LED_PIN 26
// #define BTN_RESET_PIN 32
// #define WF_LED_PIN 33
//out
// mqtt server
//#define MQTT_SERVER "192.168.1.90"// server
#define MQTT_SERVER "192.168.43.91"// server
#define MQTT_PORT 1883//port
// sub
#define MQTT_DOOR_STATE_PUB "door_state_out"
//pub
#define MQTT_DOOR_STATE_SUB "door_state_in"
// fb
#define FIREBASE_DOOR "/Smart_home/outside/door_status"




typedef enum{
    NONE            =0,
    ADD_CARD        =1,
    DEL_CARD        =2,
    CHANGE_MASTER   =3,
    CHANGE_PASS     =4,
    DELETE_ALL_CARD  =5,
} setting_mode_t;

#define PASS_STORAGE_ADDR 50
#define PASS_LENGTH_MAX 10
#define PASS_LENGTH_MIN 4
