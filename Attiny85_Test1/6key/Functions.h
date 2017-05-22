/*
 * Functions.h
 *
 * Created: 2017/5/2 22:26:44
 *  Author: Administrator
 */ 

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <avr/io.h>
#include "usbkeyboard.h"
 #ifdef SIXKEY
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1


#ifdef __cplusplus
} // extern "C"
#endif
uint8_t releasekey(uint8_t key);
uint8_t presskey(uint8_t key);
void releaseAll();
void pressModifierKeys(uint8_t key);
void releaseModifierKeys(uint8_t key);
void pinMode(uint8_t IO,uint8_t value);
void digitalWrite(uint8_t IO,uint8_t value);
uint8_t digitalRead(uint8_t IO);
void keyPrintWord(char * word);
void keyPrintWord2(void);
int getwords2length();
int setup();
#ifdef __cplusplus
} // extern "C"
#endif
extern const  uint8_t  words2[] PROGMEM;
#endif
#endif /* FUNCTIONS_H_ */