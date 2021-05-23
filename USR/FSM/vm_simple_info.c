/*
简易查询界面

主要提供给快捷键查询

界面规划：
==========================================
1页：
1行-当前温度 //目标温度 //待发数据
2行-版本号
3行-驱动板版本
4行-机器ID
2页：
==========================================
*/


#include "header.h"

uint8_t fsm_fun_vmSimpleInfo(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				SETUP_EX1 pt_ex1Set;
				
				lcd_clear();
				//目标温度
				e2p_get_ex1(&pt_ex1Set);
				lcd_show_num(pt_ex1Set.tempTarget, pt_word.warring[19].x, pt_word.warring[19].y, pt_word.warring[19].color, pt_word.warring[19].background, *(pt_word.warring[19].font), 1, 2);
				lcd_show_character(&pt_word.warring[23]);
				
				if(pt_driverBoard[0]->temp.state[0] == 1)
				{
					//当前温度值
					if(pt_driverBoard[0]->temp.value[0] > 127 )//负温度
					{
						uint8_t tmp;
						tmp = (~pt_driverBoard[0]->temp.value[0])+1;
						lcd_show_string("-",pt_word.warring[19].valx, pt_word.warring[19].valy, pt_word.warring[19].color, pt_word.warring[19].background, *(pt_word.warring[19].font));
						lcd_show_num(tmp,  pt_word.warring[19].valx + 8, pt_word.warring[19].valy, pt_word.warring[19].color, pt_word.warring[19].background, *(pt_word.warring[19].font), 1, 2);
					}
					else
					{
						lcd_show_num(pt_driverBoard[0]->temp.value[0], pt_word.warring[19].valx, pt_word.warring[19].valy, pt_word.warring[19].color, pt_word.warring[19].background, *(pt_word.warring[19].font), 1, 2);
					}
					//摄氏度图标
					lcd_show_string(pt_word.warring[23].character, pt_word.warring[23].valx, pt_word.warring[23].valy, pt_word.warring[23].color, pt_word.warring[23].background, *(pt_word.warring[23].font));
				}
				else//未装温度探头或探头故障
				{
					//未装探头图标
					lcd_show_string(pt_word.warring[19].character, pt_word.warring[23].valx, pt_word.warring[23].valy, pt_word.warring[23].color, pt_word.warring[23].background, *(pt_word.warring[23].font));
				}
				
				//待发数据图标
				lcd_show_character(&pt_word.warring[22]);
				//待发数据
				lcd_show_num(mailnum_get(), pt_word.warring[22].valx, pt_word.warring[22].valy, pt_word.warring[22].color, pt_word.warring[22].background, *(pt_word.warring[22].font), 0, 10);
				//主板版本号
				lcd_show_string((const char*) version, pt_word.info[0].valx, pt_word.info[0].valy, pt_word.info[0].color, pt_word.info[0].background, *(pt_word.info[0].font));
				//驱动板版本号
				lcd_show_string((const char *)pt_driverBoard[0]->version, pt_word.info[4].valx, pt_word.info[4].valy, pt_word.info[4].color, pt_word.info[4].background, *(pt_word.info[4].font));
				//机器ID
				{
					SETUP_DEVICE mac_data;
					e2p_get_vminfo(&mac_data);
					lcd_show_character(&pt_word.info[3]);
					lcd_show_num(mac_data.facId, pt_word.info[3].valx, pt_word.info[3].valy, pt_word.info[3].color, pt_word.info[3].background, *(pt_word.info[3].font), 1, 10);
				}
				//显示机柜数量
				
				//纸币器
				
				//硬币器
				
				//读卡器
//				if(cIC.state == 1)
//				{
//					lcd_show_character(&pt_word.warring[24]);
//				}
				
			}
			else
			if(cmsg->stype == FSM_STATUS_TIMEOUT)
			{
				Load_status(fsm_id_init);
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_init);
			}
		break;
	}
	return 0;
}
