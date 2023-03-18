EEPROM
50: do dai pass (255 la khong co pass)
51->60: pass
////////////////////////////////////////////
#include<Arduino.h>
#include<M_KEYPAD.h>
//make keypad
byte COLS_PIN[ROW]={13,14,27,26};
byte ROWS_PIN[COL]={25,33,32,12};
char password[PASS_LENGTH_MAX];// in keypad
uint8_t keyCnt;
Keypad myKeypad= Keypad( makeKeymap(keys), ROWS_PIN, COLS_PIN, ROW, COL );
void keypad_setup(){
    // bat cai dat mk
}
// check 
void keypad_loop(){
    //keypad_read_password(0);
    if(keypad_setup_pass()) Serial.println("good");
   
}
char keypad_getkey(){
    return myKeypad.getKey();
}
bool keypad_read_password(bool isSetup){
    bool finish=false;
    char key=myKeypad.getKey();
    if(key){
        switch (key)
        {
        case '*':
            keypad_clear_pass();
            break;
        case '#':
            if(!isSetup){
            finish =keypad_is_password();
            keypad_clear_pass();
            }else finish=true;
            break;
        default:
            keypad_append_pass(key);
            break;
        }
        //test
        Serial.print("\npass= ");
        for(int i=0;i<keyCnt;i++){
            Serial.print(password[i]);
        }
    }
    
    if(finish) Serial.println("ok");
    return finish;
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
bool keypad_is_password(){
    // neu chua co pass-> xac nhan
    char tArr[6]={'1','2','3','4','5','6'};
    if(keyCnt!=6) return false;
    for(uint8_t i=0;i<keyCnt;i++){
        if(password[i]!=tArr[i]) return false;
    }
    return true; 
}
void keypad_delete_pass(){
    
}
bool keypad_setup_pass(){
    static char firstArr[PASS_LENGTH_MAX];
    static uint8_t cnt1=0;
    if(cnt1==0){
    if(keypad_read_password(1)){
         cnt1=keyCnt;
         keypad_coppy_arr(firstArr,password,cnt1);
         keypad_clear_pass();
        }
        return false;
        } else{
            if(keypad_read_password(1)){
                if(cnt1!=keyCnt){
                    cnt1=0;
                    keypad_clear_pass();
                    return false;
                }
                if(!keypad_comp_arr(firstArr,password,keyCnt)){
                    cnt1=0;
                    keypad_clear_pass();
                    return false;
                }
        } else return false;
    }
    cnt1=0;
// luwu
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
