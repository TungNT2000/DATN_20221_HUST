
#include <MFRC522.h>
#define SS_PIN 21
#define RST_PIN 4
#define UID_STORAGE_ADDR 0
typedef enum{
    rfid_block         =0,
    rfid_read          =1,
    rfid_write_member  =2,
    rfid_del_member    =3,
    rfid_write_master  =4,
    rfid_del_master    =5,
    rfid_del_all_member=6,
}rfid_state_t;
typedef enum{
    rfid_ev_none       =0,
    rfd_ev_success     =1,
    rfid_ev_false      =2,
}rfid_ev_state_t;
void rfid_setup();
void rfid_loop();
bool rfid_read_card();
byte *rfid_get_uid_card();
bool  rfid_write_member_uid(uint8_t memUid[]);
bool rfid_write_master_uid(uint8_t masUid[]);
bool rfid_delete_member_uid(uint8_t memUid[]);
bool rfid_delete_master_uid();
bool rfid_delete_all_member_uid();
bool rfid_uid_is_master(uint8_t Uid[]);
bool rfid_uid_is_storage(uint8_t Uid[]);

//set state
void rfid_set_state(rfid_state_t state);
rfid_state_t rfid_get_state();
rfid_ev_state_t rfid_get_ev_state();
///prv
bool rfid_comp_arr(byte arr1[], byte arr2[]);
void  rfid_write_eeprom(uint8_t num , uint8_t arrData[]);
uint8_t  rfid_find_num_uid(uint8_t arr[]);
//test
void write_eep(uint8_t num);
void eep_end();