/*
�ֿ�ͷ�ļ�
*/

#ifndef __FONT_H__
#define __FONT_H__

#include "stdint.h"

#define FONT_48		48
#define FONT_64		64
#define FONT_96		96

enum LCD_TYPE{LCD_HZ , LCD_EN};//����/Ӣ��

//����ASCII��
//ƫ����32
//ASCII�ַ���
//ƫ����32
extern const unsigned char asc2_48[];
extern const char LT_48[];
extern const unsigned char asc2_64[];
extern const char LT_64[];
extern const unsigned char asc2_96[];
extern const char LT_96[];



#endif
