#include<M_DEFINE.h>
void led_setup(){
  ledcSetup(LED1_CHANNEL, FREQ, RESOLUTION);
  ledcSetup(LED2_CHANNEL, FREQ, RESOLUTION);
  ledcSetup(LED3_CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(LED1_PIN, LED1_CHANNEL);
  ledcAttachPin(LED2_PIN, LED2_CHANNEL);
  ledcAttachPin(LED3_PIN, LED3_CHANNEL);
 }
void led1_set_dimmer(uint8_t dutyCycle){
  uint8_t duty= map(dutyCycle, 0, 100, 0, 255);
   ledcWrite(LED1_CHANNEL, duty);
 }
void led2_set_dimmer(uint8_t dutyCycle){
  uint8_t duty= map(dutyCycle, 0, 100, 0, 255);
  ledcWrite(LED2_CHANNEL, duty);
 }
void led3_set_dimmer(uint8_t dutyCycle){
  uint8_t duty= map(dutyCycle, 0, 100, 0, 255);
  ledcWrite(LED3_CHANNEL, duty);
 }