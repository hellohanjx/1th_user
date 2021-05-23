/*
售货机信息
详细的售货机设备信息
主要用来给维修人员诊断

这个是通用模板的一个变形

注意:
e2prom 型号(没有唯一识别号)

*/

#include "header.h"
#include "flash_struct.h" 

#define HOLDER_SIZE				44			//此菜单内容数量（添加新的菜单项要改变这个大小）
/*
定义一个结构体,是内容的集合
说明：定义为一个结构体是很重要的，
保证了内存是连续的
*/
typedef struct AGGREGATE{
	CONTENT content[HOLDER_SIZE];//添加新内容要注意这里
}AGGREGATE;

static AGGREGATE *content ;//指向内容的集合


/*
释放显示内容
如果是_malloc的全局指针，则在这里释放指针内容
如果只是一次打印到屏幕上的内容，则空着就好
*/
static void content_free(void)
{
	_free(content);
}


/*
创建显示内容
*/
static void content_creat(void)
{	
	uint8_t err;
	SETUP_DEVICE macinfo;
	SETUP_COMMON vm_common;
	
	SETUP_COMMUCATION commucation;
	
	content = _malloc(sizeof(AGGREGATE), &err); //给内容分配空间
	assert_param(content);


	e2p_get_vminfo(&macinfo);
	e2p_get_common(&vm_common);
	e2p_get_commucation(&commucation);
	
	
	
	//主板软件版本
	//id
	//spi flash容量
	
	
	//驱动板版本
	//温度1
	//温度2
	//控制回路1
	//控制回路2
	//控制回路3
	//控制回路4
	//控制回路5
	//控制回路6
	
	//读卡器状态
	
	//网络状态
	//连网密码
	//分配的数据存储区大小
	//已用空间
	//读序号
	//写序号
	//交易号
	//流水号

	//硬币器状态
	//硬币器层级
	//硬币器小数点位数
	//硬币器国家代码
	//硬币器通道
	//硬币器面值
	//硬币筒金额
	
	//纸币器状态
	//纸币器层级
	//纸币器小数点位数
	//纸币器国家代码
	//纸币接收通道
	//钞箱面值
	//纸币金额
	//暂存箱接收通道
	//找零箱接收币值
	//找零箱接收通道
	
	//cpu id，这个有96位数，放到最后

	content->content[0].item = &pt_word.info[0];	//"主板-"
	content->content[0].valueX = pt_word.info[0].valx;content->content[0].valueY = pt_word.info[0].valy;
	content->content[0].val[0] = '\0';
	
	content->content[1].item = &pt_word.info[1];	//"版本:"
	content->content[1].valueX = pt_word.info[1].valx;content->content[1].valueY = pt_word.info[1].valy;
	strcpy(content->content[1].val, (const char*)version);//内存拷贝
	
	content->content[2].item = &pt_word.info[2];	//"FLASH":
	content->content[2].valueX = pt_word.info[2].valx;content->content[2].valueY = pt_word.info[2].valy;
	{
		uint32_t tmp;
		sst26vf032b_Jedec_ID_Read(&tmp);
		transfer_value_to_char(tmp, content->content[2].val);
	}

	content->content[3].item = &pt_word.info[3];	//"ID:"
	content->content[3].valueX = pt_word.info[3].valx;content->content[3].valueY = pt_word.info[3].valy;
	transfer_value_to_char(macinfo.facId , content->content[3].val);

	
	
	content->content[4].item = &pt_word.info[4];	//"驱动板-"
	content->content[4].valueX = pt_word.info[4].valx;content->content[4].valueY = pt_word.info[4].valy;
	content->content[4].val[4] = '\0';
	
	content->content[5].item = &pt_word.info[5];	//"版本:"
	content->content[5].valueX = pt_word.info[5].valx;content->content[5].valueY = pt_word.info[5].valy;
	strcpy(content->content[5].val, (char*)pt_driverBoard[0]->version);

	content->content[6].item = &pt_word.info[6];	//"温度1:"
	content->content[6].valueX = pt_word.info[6].valx;content->content[6].valueY = pt_word.info[6].valy;
	if(pt_driverBoard[0]->temp.state[0] == 1)
	{
		transfer_value_to_char(pt_driverBoard[0]->temp.value[0], content->content[6].val);
	}
	else
	{
		strcpy(content->content[6].val, pt_word.warring[19].character);
	}
	
	content->content[7].item = &pt_word.info[7];	//"温度2:"
	content->content[7].valueX = pt_word.info[7].valx;content->content[7].valueY = pt_word.info[7].valy;
	if(pt_driverBoard[0]->temp.state[1] == 1)
	{
		transfer_value_to_char(pt_driverBoard[0]->temp.value[1], content->content[7].val);
	}
	else
	{
		strcpy(content->content[7].val, pt_word.warring[19].character);
	}
		
	content->content[8].item = &pt_word.info[8];	//"压缩机:"
	content->content[8].valueX = pt_word.info[8].valx;content->content[8].valueY = pt_word.info[8].valy;
	
	content->content[9].item = &pt_word.info[9];	//"风机:"
	content->content[9].valueX = pt_word.info[9].valx;content->content[9].valueY = pt_word.info[9].valy;
	
	content->content[10].item = &pt_word.info[10];	//"照明:"
	content->content[10].valueX = pt_word.info[10].valx;content->content[10].valueY = pt_word.info[10].valy;
	
	content->content[11].item = &pt_word.info[11];	//"消毒":
	content->content[11].valueX = pt_word.info[11].valx;content->content[11].valueY = pt_word.info[11].valy;
	
	content->content[12].item = &pt_word.info[12];	//"备用1":
	content->content[12].valueX = pt_word.info[12].valx;content->content[12].valueY = pt_word.info[12].valy;
	
	content->content[13].item = &pt_word.info[13];	//"备用2":
	content->content[13].valueX = pt_word.info[13].valx;content->content[13].valueY = pt_word.info[13].valy;
	
	
	
	content->content[14].item = &pt_word.info[14];	//"读卡器->":
	content->content[14].valueX = pt_word.info[14].valx;content->content[14].valueY = pt_word.info[14].valy;

	content->content[15].item = &pt_word.info[15];	//"状态":
	content->content[15].valueX = pt_word.info[15].valx; content->content[15].valueY = pt_word.info[15].valy;
	if(cIC.state == 1)
	{
		strcpy(content->content[15].val, (char*)pt_word.menu[14].character);
	}
	else
	{
		strcpy(content->content[15].val, (char*)pt_word.menu[15].character);
	}
	
	
	content->content[16].item = &pt_word.info[16];	//"网络->":
	content->content[16].valueX = pt_word.info[16].valx;content->content[16].valueY = pt_word.info[16].valy;

	content->content[17].item = &pt_word.info[17];	//"状态":
	content->content[17].valueX = pt_word.info[17].valx;content->content[17].valueY = pt_word.info[17].valy;
	if(g_vm.linkState == 1)
	{
		strcpy(content->content[17].val, (char*)pt_word.menu[20].character);
	}
	else
	{
		strcpy(content->content[17].val, (char*)pt_word.menu[21].character);
	}

	content->content[18].item = &pt_word.info[18];	//"待发数据":
	content->content[18].valueX = pt_word.info[18].valx;content->content[18].valueY = pt_word.info[18].valy;
	transfer_value_to_char(mailnum_get(), content->content[18].val);
	
	content->content[19].item = &pt_word.info[19];	//"存储":(可用于存数据的空间)
	content->content[19].valueX = pt_word.info[19].valx;content->content[19].valueY = pt_word.info[19].valy;
	transfer_value_to_char(FLASH_QUENE_SPACE, content->content[19].val);

	content->content[20].item = &pt_word.info[20];	//"已用":
	content->content[20].valueX = pt_word.info[20].valx;content->content[20].valueY = pt_word.info[20].valy;
	transfer_value_to_char(mailusedsec_get()*__F032B_SEC_SIZE, content->content[20].val);
	
	content->content[21].item = &pt_word.info[21];	//"读序号":
	content->content[21].valueX = pt_word.info[21].valx;content->content[21].valueY = pt_word.info[21].valy;
	transfer_value_to_char(flashqueue_head_get(), content->content[21].val);

	content->content[22].item = &pt_word.info[22];	//"写序号"
	content->content[22].valueX = pt_word.info[22].valx;content->content[22].valueY = pt_word.info[22].valy;
	transfer_value_to_char(flashqueue_rear_get(), content->content[22].val);

	content->content[23].item = &pt_word.info[23];	//"交易号":
	content->content[23].valueX = pt_word.info[23].valx;content->content[23].valueY = pt_word.info[23].valy;
	transfer_value_to_char(tradenum_get(), content->content[23].val);

	content->content[24].item = &pt_word.info[24];	//"流水号:"
	content->content[24].valueX = pt_word.info[24].valx;content->content[24].valueY = pt_word.info[24].valy;
	transfer_value_to_char(mailcount_get(), content->content[24].val);



	content->content[25].item = &pt_word.info[25];	//"硬币器->":
	content->content[25].valueX = pt_word.info[25].valx;content->content[25].valueY = pt_word.info[25].valy;

	content->content[26].item = &pt_word.info[26];	//"状态":
	content->content[26].valueX = pt_word.info[26].valx;content->content[26].valueY = pt_word.info[26].valy;
	if(coinequip.eqiu_status < 2)
	{
		strcpy(content->content[26].val, (char*)pt_word.menu[14].character); 
	}
	else
	{
		strcpy(content->content[26].val, (char*)pt_word.menu[15].character);
	}
	
	content->content[27].item = &pt_word.info[27];	//"层级":
	content->content[27].valueX = pt_word.info[27].valx;content->content[27].valueY = pt_word.info[27].valy;

	content->content[28].item = &pt_word.info[28];	//"小数":
	content->content[28].valueX = pt_word.info[28].valx;content->content[28].valueY = pt_word.info[28].valy;

	content->content[29].item = &pt_word.info[29];	//"国家代码":
	content->content[29].valueX = pt_word.info[29].valx;content->content[29].valueY = pt_word.info[29].valy;

	content->content[30].item = &pt_word.info[30];	//"通道:"
	content->content[30].valueX = pt_word.info[30].valx;content->content[30].valueY = pt_word.info[30].valy;
	
	content->content[31].item = &pt_word.info[31];	//"面值:"
	content->content[31].valueX = pt_word.info[31].valx;content->content[31].valueY = pt_word.info[31].valy;
	
	content->content[32].item = &pt_word.info[32];	//"金额:"
	content->content[32].valueX = pt_word.info[32].valx;content->content[32].valueY = pt_word.info[32].valy;
	
	
	
	content->content[33].item = &pt_word.info[33];	//"纸币器->"
	content->content[33].valueX = pt_word.info[33].valx;content->content[33].valueY = pt_word.info[33].valy;
	
	content->content[34].item = &pt_word.info[34];	//"状态:"
	content->content[34].valueX = pt_word.info[34].valx;content->content[34].valueY = pt_word.info[34].valy;
	if(billequip.eqiu_status < 2)
	{
		strcpy(content->content[34].val, (char*)pt_word.menu[14].character);
	}
	else
	{
		strcpy(content->content[34].val, (char*)pt_word.menu[15].character);
	}
	
	content->content[35].item = &pt_word.info[35];	//"层级:"
	content->content[35].valueX = pt_word.info[35].valx;content->content[35].valueY = pt_word.info[35].valy;
	
	content->content[36].item = &pt_word.info[36];	//"小数:"
	content->content[36].valueX = pt_word.info[36].valx;content->content[36].valueY = pt_word.info[36].valy;
	
	content->content[37].item = &pt_word.info[37];	//"国家代码:"
	content->content[37].valueX = pt_word.info[37].valx;content->content[37].valueY = pt_word.info[37].valy;
	
	content->content[38].item = &pt_word.info[38];	//"通道:"
	content->content[38].valueX = pt_word.info[38].valx;content->content[38].valueY = pt_word.info[38].valy;
	
	content->content[39].item = &pt_word.info[39];	//"面值:"
	content->content[39].valueX = pt_word.info[39].valx;content->content[39].valueY = pt_word.info[39].valy;
	
	content->content[40].item = &pt_word.info[40];	//"金额:"
	content->content[40].valueX = pt_word.info[40].valx;content->content[40].valueY = pt_word.info[40].valy;
	
	content->content[41].item = &pt_word.info[41];	//"暂存箱通道:"
	content->content[41].valueX = pt_word.info[41].valx;content->content[41].valueY = pt_word.info[41].valy;
	
	content->content[42].item = &pt_word.info[42];	//"找零箱面值:"
	content->content[42].valueX = pt_word.info[42].valx;content->content[42].valueY = pt_word.info[42].valy;

	content->content[43].item = &pt_word.info[43];	//"找零箱金额:"
	content->content[43].valueX = pt_word.info[43].valx;content->content[43].valueY = pt_word.info[43].valy;

	//菜单配置
	holder.size = HOLDER_SIZE;					//容器大小
	holder.font = G_HOLDER_FONT; 				//容器字体
	holder.shownumPage = G_HOLDER_ITEM_SIZE+1;	//一页显示多少个内容	//这里不需要提示占一行，所以一页可以多显示1行
	holder.zicolor = G_HOLDER_CUR_ZI_COLOR;		//当前选项数字颜色
	holder.numcolor = G_HOLDER_CUR_NAME_COLOR;	//当前选项名称颜色
	holder.foreground = G_HOLDER_FOREGROUND;	//前景色
	holder.background = G_HOLDER_BACKGROUND;	//背景色
	holder.contentList = content->content;		//内容指针
	contant_load(&holder);
}
/*
定义一个函数指针数组
*/
typedef void (*display[HOLDER_SIZE])(void); 

uint8_t fsm_fun_vmInfo(FSMMSG* cmsg)
{
	switch (cmsg->type)
	{
		case MsgSYS:
			if(cmsg->stype == FSM_STATUS_INIT)
			{
				LCD_CLEAR_SIDE;
				content_creat();
			}
			else
			if(cmsg->stype == FSM_STATUS_EXIT)
			{
				content_free();			
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == DOOR_CLOSE_VAL)
			{
				Load_status(fsm_id_init);
			}
			else
			if(cmsg->value == '*')
			{
				content_free();
				Load_status(fsm_id_seniorMenu);
			}
			else 
			if(cmsg->value == KEY_DOWN_VAL)//下翻
			{
				holder.curSelectId +=  G_HOLDER_ITEM_SIZE;
				if(holder.curSelectId >= HOLDER_SIZE)
					holder.curSelectId = 0;
				content_page_down(&holder);
			}
			else
			if(cmsg->value == KEY_UP_VAL)//上翻
			{
				if(holder.curSelectId >= G_HOLDER_ITEM_SIZE)
				{
					content_page_up(&holder);
					holder.curSelectId -=  G_HOLDER_ITEM_SIZE;
				}
				else
				{
					Load_status(fsm_id_vmInfo);
				}
			}

		break;
			
	}
	return 0;
}


