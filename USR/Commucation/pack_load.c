/*
连网登录包

*/
#include "header.h"

//注：结构分隔符 * ; 包体内分隔符 '+'
//=============================
//登录包1
//设备识别号
//=============================
void get_link_info(volatile uint8_t *str)
{
	char tmp[10];
	uint8_t i, j;
	SETUP_DEVICE  vmInfomation;
	SETUP_COMMUCATION  vmCommucation; 
	
	
	i = 0;
	str[i++]=0x1f;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x30;
	str[i++]='*';//分隔符
	
	//联机ID
	e2p_get_vminfo(&vmInfomation);
	sprintf(tmp, "%10u", vmInfomation.facId);
	for(j = 0;j < 10; j++)
	{
		str[i++]=tmp[j];
	}
	str[i++]='*';
	
	//联机密码
	e2p_get_commucation(&vmCommucation);
	sprintf(tmp, "%06u", vmCommucation.password);
	for(j = 0; j < 6; j++)
	{
		str[i++]=tmp[j];
	}
	str[i++]='*'; 
	
	//分钟
	sprintf(tmp, "%02u", calendar.min);
	for(j = 0; j < 2; j++)
	{
		str[i++]=tmp[j];
	}
	//秒
	sprintf(tmp, "%02u", calendar.sec);
	for(j = 0; j < 2; j++)
	{
		str[i++]=tmp[j];
	}
	str[i++]='*'; 
	str[2] = i+1;
}
//=============================
//登录包2
//机器货道状态
//=============================
void get_channel_status(volatile uint8_t* str)
{
	char tmp[10];
	uint16_t i, j, len;
	uint8_t container, layer, column, state;
	uint8_t validColumnNum;	//使能货道数量
	uint8_t errColumnNum;	//故障货道数量
	SETUP_CHANNEL  channel;

	container = 0;
//	layer = 8;
	
	i=0;
	str[i++]=0x1f;
	str[i++]='*';
	str[i++]='*'; 
	str[i++]=0x31;
	str[i++]='*';
	//机柜号
	str[i++] = container + '1';
	str[i++] = '*';
	//层总数
	str[i++] = g_vm.maxtrayNum + '0';
	str[i++] = '*';
	for(container = 0;container < g_vm.maxContainerNum ; container++)
	{
		for(layer = 0; layer < g_vm.maxtrayNum; layer++)//一次传输所有层信息(可更改) 
		{
			//查询当前层
			validColumnNum = 0;
			errColumnNum = 0;
			
			//读取货道使能状态
			for(column = 0;column < 10; column++)
			{
				e2p_get_channelEnable (container*100 + layer*10 + column, &state);//读取对应的参数
				if(state == 1)//货道使能
				{
					validColumnNum++;//计算使能货道总数
					if(pt_driverBoard[container]->channelState[10*layer+column] != 1)//除了正常都是不正常，不分类
					{
						errColumnNum ++;//故障货道总数
					}
				}
			}
			//这个是针对整托盘的情况，托盘信息都存储在0货道中
			e2p_get_channel(container*100 + layer*10 ,&channel);
			
			//层号
			len = sprintf(tmp,"%u",(layer+1));
			for(j = 0;j < len ;j++)
			str[i++] = tmp[j];
			str[i++] = '*';
			//当前层货道数量（使能货道）
			len = sprintf(tmp,"%u",validColumnNum);
			for(j = 0;j < len ;j++)
			str[i++] = tmp[j];
			str[i++] = '*';
			//状态（无故障货道为0，1个故障为1...）
			len = sprintf(tmp,"%u",errColumnNum);
			for(j = 0;j < len ;j++)
			str[i++] = tmp[j];
			str[i++] = '*';
			//货道商品编码
			sprintf(tmp, "%06u", channel.code);
			for(j = 0; j < 6; j++)
			str[i++] = tmp[j]; 
			str[i++] = '*';
			//货道销售价格
			len = sprintf(tmp, "%u", channel.price );
			for(j = 0;j < len;j++)
			str[i++] = tmp[j];
			str[i++] = '*';
			//货道库存
//			len = sprintf(tmp, "%u", channel.store );
			{
				//托盘库存
			uint8_t t_num;
			uint16_t t_val;
			bkp_trayStatistic_get(layer, &t_num, &t_val);
			len = sprintf(tmp, "%u",  channel.store - t_num);
			for(j = 0;j < len;j++)
			str[i++] = tmp[j];
			str[i++] = '*';
			}
			//货道弹簧圈数
			len = sprintf(tmp,"%u",channel.spring);
			for(j = 0;j < len;j++)
			str[i++] = tmp[j];
			
			if(layer != (g_vm.maxtrayNum - 1) )//最后一层不要加分隔符
			str[i++] = '*';
		}
	}
	str[i++] = '*';
	str[2] = i+1;;
}
//================================
//登录包3
//整机状态
//================================
void get_machine_status(volatile uint8_t* str)
{
	char tmp[10];
	uint32_t i,j,len;
	uint8_t validEquNum;//本次上报的设备数量
	
	validEquNum = 0;
	i = 0;
	str[i++]=0x1f;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x34;
	str[i++]='*';
	
	//本次上报设备数量
	str[i++] = '8';
	str[i++] = '*';
	//驱动板
	str[i++] = '0';
	str[i++] = '1';
	str[i++] = '*';
	if(pt_driverBoard[0]->state == 1)
		str[i++] = '0';
	else
		str[i++] = '1';
	str[i++] = '*';
	validEquNum++;
	//门状态
	str[i++] = '0';
	str[i++] = '2';
	str[i++] = '*';
	str[i++] = g_vm.door + '0';
	str[i++] = '*';
	validEquNum++;
	//读卡器
	str[i++] = '0';
	str[i++] = '3';
	str[i++] = '*';
	if(cIC.state == 1)
		str[i++] = '0';
	else
		str[i++] = '1';
	str[i++] = '*';
	validEquNum++;
	//温度
	str[i++] = '0';
	str[i++] = '4';
	str[i++] = '*';
	len = sprintf(tmp, "%d", pt_driverBoard[0]->temp.value[0]);
	for(j = 0;j < len;j++)
		str[i++] = tmp[j];
	str[i++] = '*';
	validEquNum++;
	//制冷
	str[i++] = '0';
	str[i++] = '5';
	str[i++] = '*';
	if(pt_driverBoard[0]->exState[EX_COLD] == 1)
		str[i++] = '1';
	else
		str[i++] = '0';
	
	str[i++] = '*';
	validEquNum++;
	//风机
	str[i++] = '0';
	str[i++] = '6';
	str[i++] = '*';
	if(pt_driverBoard[0]->exState[EX_WIND] == 1) 
		str[i++] = '1';
	else
		str[i++] = '0';
	str[i++] = '*';
	validEquNum++;

	//日光灯
	str[i++] = '0';
	str[i++] = '7';
	str[i++] = '*';
	if(pt_driverBoard[0]->exState[EX_LIGHT] == 1)
	str[i++] = '1';
	else
	str[i++] = '0';
	str[i++] = '*';
	validEquNum++;

	//消毒灯
	str[i++] = '0';
	str[i++] = '8';
	str[i++] = '*';
	if(pt_driverBoard[0]->exState[EX_DISINFECT] == 1)
	str[i++] = '1';
	else
	str[i++] = '0';
	str[i++] = '*';
	validEquNum++;

	str[2] = i+1;
}
//=========================
//登录包4
//程序版本号
//=========================
void get_software_version(volatile uint8_t* str)
{
	uint8_t i,j;
	
	i = 0;
	str[i++]=0x1f;
	str[i++]='*';
	str[i++]='*';
	str[i++]=0x35;
	str[i++]='*';
	//包体
	for(j=0; version[j] != '\0'; j++)//软件版本号
		str[i++] = version[j];
	str[i++] = '*';
	
	if(pt_driverBoard[0]->state == 1 )
	{
		for(j=0; pt_driverBoard[0]->version[j] != '\0'; j++)//软件版本号
		{
			str[i++] = pt_driverBoard[0]->version[j];
		}
	}
	else
		str[i++] = '0';

	str[i++] = '*';
	str[2] = i+1;
}
