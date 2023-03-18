#include <Arduino.h>
#include <SPI.h>
#include<M_RFID.h>
#include<EEPROM.h>
const byte eepVal =EEPROM.read(90);// gia tri mac dinh cua eeprom
MFRC522 rfid(SS_PIN, RST_PIN); 
uint8_t nuidPICC[4];
rfid_state_t rfidState=rfid_read;
rfid_ev_state_t rfidEvState=rfid_ev_none;

void rfid_setup(){
    SPI.begin(); // Init SPI bus
    //
    if(EEPROM.read(0)==255){
        EEPROM.write(0,0);
        EEPROM.commit();
    }
    if(EEPROM.read(1)==255){
        EEPROM.write(1,0);
        EEPROM.commit();
    }
    //
    Serial.println(EEPROM.read(0));
    Serial.println(EEPROM.read(1));
    rfid.PCD_Init(); // Init MFRC522 
    Serial.println(F("RFID setup ok."));
    write_eep(EEPROM.read(1));
    // bat cai the master
}
void rfid_loop(){
    // if(rfidState!=rfid_block){
    //     if(rfid_read_card()){   
    //         switch (rfidState)
    //         {
    //         case rfid_read:
    //             /* code */
    //             // dong mo cua
    //             break;
    //         case rfid_write_member:
    //             /* code */
    //             // them tv
    //             rfid_write_member_uid(nuidPICC);
    //             break;
    //         case rfid_del_member:
    //             /* code */
    //             // xoa tv
    //             break;
    //         case rfid_write_master:
    //             /* code */
    //             //them master
    //             break;
    //         case rfid_del_master:
    //             /* code */
    //             //xoa master
    //             break;
    //         case rfid_del_all_member:
    //             /* code */
    //             //xoa tat ca tv
    //             break;
    //         default:
    //             break;
    //         }
    //     }
    // }
}
// detect and read card
bool rfid_read_card(){
    if ( ! rfid.PICC_IsNewCardPresent())
    return false;
    if ( ! rfid.PICC_ReadCardSerial())
    return false;
    for (uint8_t i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
// stop read
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return true;
}
// get nuid
byte *rfid_get_uid_card(){
    return nuidPICC; 
}
// delete nuid
void rfid_delete_nuidPICC(){
    for (uint8_t i = 0; i < 4; i++)
    {
        nuidPICC[i]=0;
    }
    
}
// function
bool  rfid_write_member_uid(uint8_t memUid[]){
    //check ........
    bool bNum=rfid_find_num_uid(memUid);
    if(bNum!=0) return false;
    // ++ num
    uint8_t cnt=EEPROM.read(UID_STORAGE_ADDR+1);
    cnt++;
    EEPROM.write(UID_STORAGE_ADDR+1,cnt);
    EEPROM.commit();
    rfid_write_eeprom(cnt,memUid); 
    //
    Serial.print(" num=");
    Serial.println(EEPROM.read(1)); 
    //
    return true;
}
bool rfid_write_master_uid(uint8_t masUid[]){
    // if(EEPROM.read(UID_STORAGE_ADDR)==1) return false;
    EEPROM.write(UID_STORAGE_ADDR,1);
    EEPROM.commit();
    rfid_write_eeprom(0,masUid);
    return true;
}
bool rfid_delete_member_uid(uint8_t memUid[]){
    uint8_t num= rfid_find_num_uid(memUid);
    if(num==0) return false;
    uint8_t cnt=EEPROM.read(UID_STORAGE_ADDR+1);
    if(cnt==0) return false;
    // uint8_t looping= (cnt-num)*4;
    // uint16_t start =num*4+2+UID_STORAGE_ADDR;
    // for(uint8_t i=0; i<looping;i++){
    //     EEPROM.write(num+i,EEPROM.read(start+i+4));
    // }
    // for(uint8_t k=0;k<4;k++){
    //     EEPROM.write(cnt*4+2+UID_STORAGE_ADDR,255);
    // }
    uint8_t start1= num*4+2+UID_STORAGE_ADDR;
    uint8_t start2= cnt*4+2+UID_STORAGE_ADDR;
    for(uint8_t k=0;k<4;k++){
        EEPROM.write(start1+k,EEPROM.read(start2+k));
        EEPROM.write(start2+k,255);
    }
    cnt--;
    EEPROM.write(UID_STORAGE_ADDR+1,cnt);
    EEPROM.commit();
    //
    Serial.print("num=");
    Serial.println(cnt);
    //
    return true;
    
}
bool rfid_delete_master_uid(){
    if(EEPROM.read(UID_STORAGE_ADDR)==0) return false;
    EEPROM.write(UID_STORAGE_ADDR,0);
    for(uint8_t i=0;i<4;i++){
        EEPROM.write(UID_STORAGE_ADDR+2+i,255);
    }
    EEPROM.commit();
    return true;
}
bool rfid_delete_all_member_uid(){
    uint8_t cnt= EEPROM.read(UID_STORAGE_ADDR+1);
    if(cnt==0) return false;
    EEPROM.write(UID_STORAGE_ADDR+1,0);
    for(int i=0 ;i<cnt*4;i++){
        EEPROM.write(UID_STORAGE_ADDR+6+i,255);
    }
    EEPROM.commit();
    return true;
}
//check uid
bool rfid_uid_is_master(uint8_t Uid[]){
    for(uint8_t i=0;i<4;i++){
        if(Uid[i]!=EEPROM.read(2+UID_STORAGE_ADDR+i)) return false;
    }
    return true;
}
bool rfid_uid_is_storage(uint8_t Uid[]){
   // if(rfid_uid_is_master(Uid)) return true;
    if(rfid_find_num_uid(Uid)>0) return true;
    return false;
}
// priv
bool rfid_comp_arr(byte arr1[], byte arr2[]){
    for(uint8_t i=0;i<4;i++){
        if(arr1[i]!= arr2[i])
        return false;
    }
    return true;
}
// master num=0
// member num =1->
void rfid_write_eeprom(uint8_t num , uint8_t arrData[]){
    uint16_t idx = num*4+2+ UID_STORAGE_ADDR;
    for(uint8_t i=0;i<4;i++){
        EEPROM.write(idx+i,arrData[i]);
        // EEPROM.commit();
    }
    EEPROM.commit();
}
// uint8_t  *rfid_read_eeprom(uint8_t num){
//     // uint8_t tmpArr[4];
//     // uint16_t idx = num*4+2+ UID_STORAGE_ADDR;
//     // for(uint8_t i=0;i<4;i++){
//     //     tmpArr[i]=EEPROM.read(idx+i);
//     // }
//     // return tmpArr;
// }
// find num uid
// fix
uint8_t  rfid_find_num_uid(uint8_t arr[]){
    uint8_t cnt= EEPROM.read(UID_STORAGE_ADDR+1);
    // if(cnt !=0){
    // uint8_t *tmpArr;
    // for(uint8_t i=1;i<=cnt;i++){
    //     // tmpArr=rfid_read_eeprom(i);
    //     // if(rfid_comp_arr(arr,tmpArr)) return i;    
    //  }
    // }
    // return 0;
    uint8_t k;// dem so lan dung
    if(cnt==0) return 0;
    for(uint8_t i=1;i<=cnt;i++){
        for(uint8_t j=0;j<4;j++){
            if(arr[j]!=EEPROM.read(UID_STORAGE_ADDR+2+i*4+j)){
                k=0;
                break;
            }
            else k++;
        }
        if(k==4){
            return i;
        }
    }
    return 0;
}
//r=test
void write_eep(uint8_t num){
  Serial.println("data= ");
  for(int i=0;i<num*4+6;i++){
    Serial.print(EEPROM.read(i),HEX);
    Serial.print(" ");
  }
}
void eep_end(){
    EEPROM.end();
}

