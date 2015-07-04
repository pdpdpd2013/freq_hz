#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char flag=0;
unsigned char scan_count=0;
unsigned char scan_digi[4];
unsigned int freq=0;
unsigned int count=0;

void port_init()
{
	DDRD=0X00;
	DDRA=0XFF;
}
void int0_init()
{
	GICR|=(1<<INT0);//int0 enable
	MCUCR|=(1<<ISC00)|(1<<ISC01);//falling generate int0
}
void timer_init()
{
//	TCCR0|=(1<<CS02)|(1<<CS00);//128 fen pin       //timer0 used as timer
//	TCNT0=0X6F; 
//	TIMSK|=(1<<TOIE0);

	TCCR1B|=(1<<CS12);//256 fen pin       //timer2 used as a tool to scan the leds
	TCNT1H=0XEF;
	TCNT1L=0X00;
	TIMSK|=(1<<TOIE1);

}
void device_init()
{
	cli();
	port_init();
	int0_init();
	timer_init();
	sei();
}

unsigned char NUMTOSEG7(unsigned char DATA)
{ unsigned char AA;
  switch (DATA)
  { case 0: AA=0xc0;break;  // ‘0’
    case 1: AA=0xf9;break;  // ‘1’
    case 2: AA=0xa4;break;  // ‘2’
    case 3: AA=0xb0;break;  // ‘3’
    case 4: AA=0x99;break;  // ‘4’
    case 5: AA=0x92;break;  // ‘5’
    case 6: AA=0x82;break;  // ‘6’
    case 7: AA=0xf8;break;  // ‘7’
    case 8: AA=0x80;break;  // ‘8’
    case 9: AA=0x90;break;  // ‘9’
    case 10: AA=0x88;break; // ‘A’
    case 11: AA=0x83;break; // ‘B’
    case 12: AA=0xc6;break; // ‘C’
    case 13: AA=0xa1;break; // ‘D’
    case 14: AA=0x86;break; // ‘E’
    case 15: AA=0x8e;break; // ‘F’
    case '-':AA=0xdf;break; // 破折号
    case '_':AA=0xf7;break; // 下划线
    case ' ':AA=0xff;break; // 消隐
	default: AA=0xff;
  }
  return(AA);
}


void serial_to_parral(unsigned char disp1,unsigned char disp2)    //本函数用于将两个8位的信号通过串口转并口输出
{                                                                 //其中，第一个参数最终传输给了U3,而第二个参数传输给了U2
	PORTA|=0X80;
	for(int q=0;q<8;q++)
	{
		if((disp1&0x80)==0) PORTA&=~0X20;
		else PORTA|=0X20;
		PORTA&=~0X40;
		PORTA|=0X40;
		disp1<<=1;
	}
	for(int w=0;w<8;w++)
	{
		if((disp2&0x80)==0) PORTA&=~0X20;
		else PORTA|=0X20;
		PORTA&=~0X40;
		PORTA|=0X40;
		disp2<<=1;
	}
	PORTA&=~0X80;
	PORTA|=0X80;
}

ISR(INT0_vect)
{
	++count;
}

ISR(TIMER1_OVF_vect)					  //timer0 used as timer
{
	sei();
	TCNT1H=0X85;
	TCNT1L=0XEC;
	freq=count;
	count=0;
}

/*ISR(TIMER2_OVF_vect)                      //timer2 used as a tool to scan the leds
{
	sei();
	TCNT2=0X6E;
	scan_digi[0]=freq/1000%10;
	scan_digi[1]=freq/100%10;
	scan_digi[2]=freq/10%10;
	scan_digi[3]=freq%10;
	if(++scan_count>4) scan_count=1;
	switch(scan_count)
	{
		case 1: serial_to_parral(0x01,NUMTOSEG7(scan_digi[0])); break;//显示高第一位
		case 2: serial_to_parral(0x02,NUMTOSEG7(scan_digi[1])); break;//显示高第二位
		case 3: serial_to_parral(0x04,NUMTOSEG7(scan_digi[2])); break;//显示高第三位
		case 4: serial_to_parral(0x08,NUMTOSEG7(scan_digi[3])); break;//显示高第四位
		default: scan_count=1; break;
	}
}*/

int main()
{
	device_init();	
	while(1)
	{
		scan_digi[0]=freq/1000%10;
		scan_digi[1]=freq/100%10;
		scan_digi[2]=freq/10%10;
		scan_digi[3]=freq%10;
		if(++scan_count>4) scan_count=1;
		switch(scan_count)
		{
			case 1: serial_to_parral(0x01,NUMTOSEG7(scan_digi[0])); break;//显示高第一位
			case 2: serial_to_parral(0x02,NUMTOSEG7(scan_digi[1])); break;//显示高第二位
			case 3: serial_to_parral(0x04,NUMTOSEG7(scan_digi[2])); break;//显示高第三位
			case 4: serial_to_parral(0x08,NUMTOSEG7(scan_digi[3])); break;//显示高第四位
			default: scan_count=1; break;
		}
	}
}

