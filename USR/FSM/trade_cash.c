/*
现金支付
*/

#include "header.h" 

enum{INIT, SELECT, NO_SELECT, SALE, DISPENSE_COIN};

uint8_t fsm_fun_cashTrade(FSMMSG* cmsg)
{
	
	switch(cmsg->type)
	{
		case MsgSYS:
			g_fsm.mode = INIT;//
			lcd_clear();
			
			lcd_show_character(&pt_word.choose[28]);//"投入"
			//投入金额
			lcd_show_money(credit_get(), pt_word.choose[28].valx, pt_word.choose[28].valy, pt_word.choose[28].color, pt_word.choose[28].background, *(pt_word.choose[28].font));
			lcd_show_character(&pt_word.choose[29]);//"商品"
			lcd_show_character(&pt_word.choose[30]);//"价格"
			
			if(select_state_get())//已选货
			{
				if(credit_get() >= g_trade.channel.price)
				{
					//先判断是否可以出货，如果可以，直接出货
					//否则提示客户
				}
				else
				{
					lcd_show_character(&pt_word.choose[31]);//"请继续投币"
				}
			}
			else//未选货
			{
				lcd_show_character(&pt_word.choose[32]);//“请选择商品”
			}
		break;
		
		case MsgKEY:
			if(cmsg->value == '*')
			{
				Load_status(fsm_id_init);
			}
			else
			{
				if(cmsg->value > '0' && cmsg->value <= '9')
				{
					select_goods(cmsg->value);
					//显示所选货道
					lcd_show_string((const char*)g_trade.select.select, pt_word.choose[29].valx, pt_word.choose[29].valy, pt_word.choose[29].color, pt_word.choose[29].background, *(pt_word.choose[29].font)); 
					if(select_state_get())
					{
						//判断货道
						//判断金额
						//提示/出货
					}
				}
			}
		
		break;
		
		case MsgCOIN:
			if(cmsg->stype == M_CS_ACCEPT)
			{
//				tradenumadd();
//				savecoinaccept(cmsg->value);
				g_trade.businessmode=0;
				lcd_show_money(credit_get(), pt_word.choose[29].valx, pt_word.choose[28].valy, pt_word.choose[28].color, pt_word.choose[28].background, *(pt_word.choose[28].font));
			}
		break;
			
		case MsgBILL:
			if(cmsg->stype == M_BS_AESC)//找零箱
			{
				ESCROW(1);
			}
			else
			if((cmsg->stype == M_BS_ABOX) || (cmsg->stype == M_BS_RECY))//钞箱
			{
				lcd_show_money(credit_get(), pt_word.choose[28].valx, pt_word.choose[28].valy, pt_word.choose[28].color, pt_word.choose[28].background, *(pt_word.choose[28].font));
			}
			else
			if(cmsg->stype == M_BS_AOUT)
			{

			}
		break;		
		
		
	}
	return 0;
}
