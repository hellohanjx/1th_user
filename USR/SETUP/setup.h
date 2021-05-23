#ifndef SETUP_H_
#define SETUP_H_

#include "stdint.h"
#include "stddef.h"

#define EEPROM_PAGE_SIZE   16  //�涨ҳ��СΪ16�ֽڣ�EEPROM��ҳ��СΪ128�ֽ� �����ǹ涨Ϊ16�ֽڲ���Ӱ��ʹ�á�
#define EEPROM_PAGE(page)  (EEPROM_PAGE_SIZE* (page))
#define DATA_VERSION 		1   //��ǰEEPROM�е����ݰ汾��
#define EEPROM_VALID  		0xAAAB		//��־
#define EEPROM_SPACE_ADDR	0xFFFF		//�ռ�β��ַ



//��0ҳ EEPROM������Ϣ
#define SETUP_FLAG_ADDR 		EEPROM_PAGE(0)   //eeprom �����Ƿ���Ч��־
typedef struct SETUP_FLAG
{
	uint16_t validFlag;
	uint16_t version;
}SETUP_FLAG;



//��1ҳ ����������Ϣ
#define SETUP_DEVICE_ADDR 		EEPROM_PAGE(1)
typedef struct SETUP_DEVICE
{
	uint32_t facId; 	//����ID
	uint32_t usrId;  	//��Ӫ������ID
	uint32_t data; 		//��������
}SETUP_DEVICE;

//��2ҳ �ۻ���ȫ������
#define SETUP_COMMON_ADDR 			EEPROM_PAGE(2)
typedef struct SETUP_COMMON 
{
	uint32_t factor; 		//�����ֵ��Сϵ����point 10��ָ�� С����Ϊ2 ��ֵΪ100
	uint8_t point; 			//С����λ��
	uint8_t language;		//����
	uint8_t drvnum;			//������Ŀ
	uint8_t traynum;		//������Ŀ
	uint8_t screenHorizontal; //�Ƿ����
	//������������λ��
	uint8_t numStore;	//���/����/��������λ��
	uint8_t numPrice;	//�۸�λ��
	uint8_t numChannel;	//����λ��
	uint8_t numGoodsId;//����λ��
}SETUP_COMMON;


//��3ҳ ���ֽ�֧������
#define SETUP_CASHLESS_ADDR  		EEPROM_PAGE(3)
typedef struct SETUP_CASHLESS
{
	uint32_t port;			//ˢ���ӿں�
	uint8_t applyEnable;	//֧����ʹ��
	
	
}SETUP_CASHLESS;

//��4ҳ �ֽ�֧������
#define SETUP_CASH_ADDR			EEPROM_PAGE(4)	
typedef struct SETUP_CASH
{
	uint8_t coinLever;		//Ӳ�����㼶
	
}SETUP_CASH;

/*
��5ҳ �����·����
*/
#define SETUP_EX1_ADDR		EEPROM_PAGE(5)
typedef struct SETUP_EX1
{
	uint16_t starttime_1;	//��1���ƻ�·��ѹ����
	uint16_t stoptime_1;
	
	uint16_t starttime_2;	//��2���ƻ�·�����
	uint16_t stoptime_2;
	
	uint16_t starttime_3;	//��3���ƻ�·������
	uint16_t stoptime_3;
	
	uint8_t tempTarget;		//Ŀ���¶�
	uint8_t workMode;		//������ʽ
}SETUP_EX1;

/*
��6ҳ �����·����
*/
#define SETUP_EX2_ADDR		EEPROM_PAGE(6)
typedef struct SETUP_EX2 	
{
	uint16_t starttime_4;	//��4���ƻ�·������
	uint16_t stoptime_4;
	
	uint16_t starttime_5;	//��5���ƻ�·������1
	uint16_t stoptime_5;
	
	uint16_t starttime_6;	//��6���ƻ�·������2
	uint16_t stoptime_6;
}SETUP_EX2;

/*
��7ҳ ͨѶ
*/
#define SETUP_COMMUCATION_ADDR 	EEPROM_PAGE(7)
typedef struct SETUP_COMMUCATION
{
	uint32_t password;		//ͨѶ����
	uint16_t port;  		//�������˿�
	uint8_t enable;  		//ͨѶʹ��
	uint8_t type; 			//ͨѶ��ʽ
}SETUP_COMMUCATION;


	




/*
��8ҳ ��������  
ÿ������80������
���֧��9������

�ṹ���С����
1��ÿ����Ա��ƫ�����������ǵ�ǰ��Ա��ռ�ڴ��С��������������Ǳ��������ڳ�Ա֮���������ֽڡ�
2�������г�Ա��С������Ϻ󣬱������жϵ�ǰ�ṹ���С�Ƿ��ǽṹ�������ĳ�Ա������С�������� ������ǻ������һ����Ա�����ֽ���䡣
*/
#define SETUP_CHANNEL_ADDR	EEPROM_PAGE(8)
typedef  struct SETUP_CHANNEL		//
{
	uint32_t price;		//�۸�
	uint32_t code;		//��Ʒ����
	uint16_t store;		//���
	uint16_t storeBkp;	//���ı���
	uint16_t spring;	//����
	uint8_t priceCHK;	//�۸�У��(�۸�ÿ�ֽں�У��)
	uint8_t type;		//ʹ�ܣ�����
}SETUP_CHANNEL;
/*
ÿ��Ԫ��ƫ�Ƶ�ַ
��Щƫ�Ƶ�ַ������ڽṹ���׵�ַ��ƫ�Ƶ�ַ
����Ԫ�����ڴ��е�ʵ��λ��
*/
#define CHANNEL_PRICEBKP_OFFSET		offsetof(SETUP_CHANNEL, priceCHK)
#define CHANNEL_STORE_OFFSET		offsetof(SETUP_CHANNEL, store)
#define CHANNEL_STOREBKP_OFFSET		offsetof(SETUP_CHANNEL, storeBkp)
#define CHANNEL_SPRING_OFFSET		offsetof(SETUP_CHANNEL, spring)
#define CHANNEL_ENABLE_OFFSET		offsetof(SETUP_CHANNEL, type)
#define CHANNEL_CODE_OFFSET			offsetof(SETUP_CHANNEL, code)



/*
����ͳ��
ͳ��ÿ���������۽������������
*/
#define SETUP_COLUMN_ADDR	EEPROM_PAGE(728)
typedef  struct SETUP_COLUMN
{
	uint32_t num;		//����
	uint32_t val;		//���
	uint8_t state;		//����״̬
}SETUP_COLUMN;

#define STATISC_NUM_OFFSET		offsetof(SETUP_COLUMN, num)
#define STATISC_VAL_OFFSET		offsetof(SETUP_COLUMN, val)
#define CHANNEL_STATE_OFFSET	offsetof(SETUP_COLUMN, state)


void e2p_poweron_init(void);
void e2p_set_ex2(SETUP_EX2* pt);
void e2p_set_ex1(SETUP_EX1* pt);
void e2p_set_cash(SETUP_CASH* pt);
void e2p_set_cashless(SETUP_CASHLESS* pt);
void e2p_set_common(SETUP_COMMON* pt);
void e2p_set_vminfo(SETUP_DEVICE *pt);
void e2p_set_commucation(SETUP_COMMUCATION* pt);

void e2p_get_commucation(SETUP_COMMUCATION* pt);
void e2p_get_ex2(SETUP_EX2* pt);
void e2p_get_ex1(SETUP_EX1* pt);
void e2p_get_cash(SETUP_CASH* pt);
void e2p_get_cashless(SETUP_CASHLESS* pt);
void e2p_get_common(SETUP_COMMON* pt);
void e2p_get_vminfo(SETUP_DEVICE *pt);


void e2p_get_global(void);
void e2p_get_channel(uint16_t channel, SETUP_CHANNEL* pt);
bool e2p_get_price(uint16_t channel, uint32_t *dat);
bool e2p_get_channelEnable(uint16_t channel, uint8_t *dat);
bool e2p_get_store(uint16_t channel, uint16_t* dat);
bool e2p_get_spring(uint16_t channel, uint16_t* dat);
bool e2p_get_channelCode(uint16_t channel, uint32_t* dat);

bool e2p_set_channel(uint16_t channel, SETUP_CHANNEL* pt);
bool e2p_set_price(uint16_t channel, uint32_t *dat);
bool e2p_set_channelEnable(uint16_t channel, uint8_t *dat);
bool e2p_set_store(uint16_t channel, uint16_t* dat);
bool e2p_set_spring(uint16_t channel, uint16_t* dat);
bool e2p_set_channelCode(uint16_t channel, uint32_t* dat);

void e2p_reset_all(void);

bool e2p_set_storeBkp(uint16_t channel, uint16_t* dat);
bool e2p_get_storeBkp(uint16_t channel, uint16_t* dat);

bool e2p_add_channelStatistic(uint16_t channel, uint32_t num, uint32_t val);
void e2p_reset_channelStatistic(void);
bool e2p_get_channelStatistic(uint16_t channel, uint32_t* num, uint32_t *val);
bool e2p_set_channelStatistic(uint16_t channel, uint32_t num, uint32_t val);

//����״̬���м���Ԫ���ź��ߵ����
bool e2p_set_channelState(uint16_t channel, uint8_t *dat);
bool e2p_get_channelState(uint16_t channel, uint8_t *dat);



#endif
