#ifndef __OLED_DATA_H
#define __OLED_DATA_H
 
#include <stdint.h>
 
/*????????*/
#define OLED_CHN_CHAR_WIDTH			2		//UTF-8?????3,GB2312?????2
 
/*??????*/
typedef struct 
{
	char Index[OLED_CHN_CHAR_WIDTH + 1];	//????
	uint8_t Data[32];						//????
} ChineseCell_t;
 
/*ASCII??????*/
extern const uint8_t OLED_F8x16[][16];
extern const uint8_t OLED_F6x8[][6];
 
/*?12??,?24??*/
extern const uint8_t OLED_F12x24[][36];
 
 
/*????????*/
extern const ChineseCell_t OLED_CF16x16[];
 
/*??????*/
extern const uint8_t Diode[];
extern const uint8_t Return[];
extern const uint8_t Frame[];
extern const uint8_t Menu_Graph[][128];
extern const uint8_t Ground[];
extern const uint8_t Barrier[][48];
extern const uint8_t Cloud[];
extern const uint8_t Dino[][48];
extern const uint8_t Eyebrow[][32];
extern const uint8_t Mouth[];
extern const uint8_t Battery[];
/*???????,???????????????*/
//...
 
#endif
 
 
/*****************????|????****************/
/*****************jiangxiekeji.com*****************/