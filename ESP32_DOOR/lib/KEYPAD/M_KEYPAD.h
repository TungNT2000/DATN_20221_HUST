#include<Keypad.h>
// #ifndef _BV
// #define _BV(bit) (1 << (bit)) 
// #endif

// #define ROW 4
// #define COL 3
#define PASS_STORAGE_ADDR 50
#define PASS_LENGTH_MAX 10
#define PASS_LENGTH_MIN 4



void keypad_setup();
void keypad_loop();
bool keypad_setup_pass();
bool keypad_read_password();
char keypad_getkey();
uint8_t *keypad_get_password_eeprom();
bool keypad_is_password();
bool keypad_write_password();//write to eeprom
char *keypad_get_pass();
/////pass
void keypad_append_pass(char key);
void keypad_clear_pass();
bool keypad_delete_pass();
void keypad_coppy_arr(char arr1[],char  arr2[2],uint8_t size);
bool keypad_comp_arr(char arr1[],char arr2[],uint8_t size);