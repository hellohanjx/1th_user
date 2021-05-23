/*
通讯架构头文件
这个文件不对外，只在本模块相关部分文件使用

*/

#ifndef _COM_FRAM_H__
#define _COM_FRAM_H__

#include "stdint.h"
#include "com_header.h"




void get_link_info(volatile uint8_t *str);
void get_channel_status(volatile uint8_t* str);
void get_machine_status(volatile uint8_t* str);
void get_software_version(volatile uint8_t* str);

void send_mail(MAIL* mymail);
void send_f_mail(MAIL* mymail);
void mail_to_flash(MAIL  *cmail);


#endif
