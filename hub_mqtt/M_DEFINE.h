//#define MQTT_SERVER "192.168.1.90"// server
#define MQTT_SERVER "192.168.43.91"
#define MQTT_PORT 1883//port
#define DEFAULT_POWER_MODE true
#define DEFAULT_DIMMER_LEVEL 80
//#define BTN_RST_PIN 13
#define BTN_RST_PIN 21
//#define BUZZER_PIN 14
#define BUZZER_PIN 18
// sub 
#define MQTT_FO_DHT_TEM         "dht_tem_out"
#define MQTT_FO_DHT_HUM         "dht_hum_out"
#define MQTT_FO_PIR1            "pir1_out"
#define MQTT_FO_PIR2            "pir2_out"
#define MQTT_FO_PIR3            "pir3_out"

//
#define MQTT_FO_GAS            "gas_out"
//
#define MQTT_TO_B1_RL1         "board1_rl1_in"
#define MQTT_TO_B1_RL2         "board1_rl2_in"
#define MQTT_TO_B1_RL3         "board1_rl3_in"
//
#define MQTT_TO_B2_RL1         "board2_rl1_in"
#define MQTT_TO_B2_RL2         "board2_rl2_in"
#define MQTT_TO_B2_RL3         "board2_rl3_in"
//
#define MQTT_TO_B2_DM1         "board2_dm1_in"
#define MQTT_TO_B2_DM2         "board2_dm2_in"
#define MQTT_TO_B2_DM3         "board2_dm3_in"
//
#define MQTT_TO_DOOR_STATE     "door_state_in"
#define MQTT_TO_DOOR_SETTING   "door_setting_in"
#define MQTT_TO_DOOR_BUZZER    "door_buzzer_in"
#define MQTT_TO_DOOR_DISPLAY   "door_display_in"
//
#define MQTT_TO_REM            "rem_in"
#define MQTT_TO_RAIN           "rain_in"
//pub
#define MQTT_FO_REM            "rem_out"
#define MQTT_FO_RAIN           "rain_out"
//
#define MQTT_FO_B1_RL1         "board1_rl1_out"
#define MQTT_FO_B1_RL2         "board1_rl2_out"
#define MQTT_FO_B1_RL3         "board1_rl3_out"
//
#define MQTT_FO_B2_RL1         "board2_rl1_out"
#define MQTT_FO_B2_RL2         "board2_rl2_out"
#define MQTT_FO_B2_RL3         "board2_rl3_out"
//
#define MQTT_FO_B2_DM1         "board2_dm1_out"
#define MQTT_FO_B2_DM2         "board2_dm2_out"
#define MQTT_FO_B2_DM3         "board2_dm3_out"
//
#define MQTT_FO_DOOR_STATE     "door_state_out"
#define MQTT_FO_DOOR_SETTING   "door_setting_out"
#define MQTT_FO_DOOR_BUZZER    "door_buzzer_out"
#define MQTT_FO_DOOR_DISPLAY   "door_display_out"
