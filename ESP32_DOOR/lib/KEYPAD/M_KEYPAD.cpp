#include<Arduino.h>
#include <Wire.h>
#include<Adafruit_MPR121.h>
#include<M_KEYPAD.h>
#include<EEPROM.h>
const byte eepVal =EEPROM.read(90);
//make keypad
// const char keys[ROW][COL]={
//   {'1', '2', '3'},
//   {'4', '5', '6'},
//   {'7', '8', '9'},
//   {'*', '0', '#'}
// };
// byte COLS_PIN[ROW]={13,12,14,27};
// byte ROWS_PIN[COL]={26,25,33};

// Keypad myKeypad= Keypad( makeKeymap(keys), ROWS_PIN, COLS_PIN, ROW, COL );
// //Adafruit_MPR121 cap = Adafruit_MPR121();
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
// check 
void keypad_loop(){
    char key2=keypad_getkey();
      if(key2) Serial.println(key2);
    //keypad_read_password(0);
//    if(EEPROM.read(PASS_STORAGE_ADDR)==255){
//     // setup
//         keypad_setup_pass();
//    }else{
//     //readpass
  //   if(keypad_read_password()) {Serial.println("dung pass");
//    }
}
// char keypad_getkey(){
//     return myKeypad.getKey();
// }
// char keypad_getkey(){
//     static uint16_t lasttouched = 0;
//     uint16_t currtouched = 0;
//     if (!cap.begin(0x5A)){
//         Serial.println("no touch");
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
// isSetup=1->setup/=0 read keypad-> save password[]
// setup tra ve 1 khi setup xong
// read tra ve 0 neu k la pass/ 1 neu la pass
// bool keypad_read_password(){
//    // static time_now;
//     static bool start =false;
//     char key=keypad_getkey();
//     if(key){
//         switch (key)
//         {
//         case '*':
//         {
//             Serial.println("bat dau nhap");
//             keypad_clear_pass();
//             start =true;
//             break;
//         }
//         case '#':
//            if(start){
//             Serial.println("hoan thanh nhap");
//             start=false;
//             return true;
//            }
           
//         default:
//             if(start) keypad_append_pass(key);
//             break;
//         }
//         //test
//         Serial.print("\npass= ");
//         for(int i=0;i<keyCnt;i++){
//             Serial.print(password[i]);
//         }
//     }
//     return false;
// }
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
bool keypad_write_password(){
    if(keyCnt==0) return false;
    EEPROM.write(PASS_STORAGE_ADDR,keyCnt);
    for(uint8_t i=0;i<keyCnt;i++){
        EEPROM.write(PASS_STORAGE_ADDR+i+1,password[i]);
    }
    EEPROM.commit();
    keypad_clear_pass();
    return true;
}
bool keypad_setup_pass(){
    // static char firstArr[PASS_LENGTH_MAX];
    // static uint8_t cnt1=0;
    // if(cnt1==0){
    // if(keypad_read_password()){
    //      cnt1=keyCnt;
    //      keypad_coppy_arr(firstArr,password,cnt1);
    //      keypad_clear_pass();
    //     }
    //     return false;
    //     } else{
    //         if(keypad_read_password(1)){
    //             if(cnt1!=keyCnt){
    //                 cnt1=0;
    //                 keypad_clear_pass();
    //                 return false;
    //             }
    //             if(!keypad_comp_arr(firstArr,password,keyCnt)){
    //                 cnt1=0;
    //                 keypad_clear_pass();
    //                 return false;
    //             }
    //     } else return false;
    // }
    // if(!keypad_write_password()) return false;
    //   cnt1=0;
    // return true;
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
