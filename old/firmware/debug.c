//ʹ��AVR Studio 4.12 + WinAVR����
//create by arcdevil @20051224
#include <avr/io.h>
#include <util/delay.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
//��������
//#define BAUDRATE        9600 //������
#define BAUDRATE 19200

unsigned char sec,min,hour;
unsigned int i = 0;
unsigned char temp;


//�򴮿�д��
void put_c(unsigned char c)
{
 while( !(UCSRA & (1<<UDRE)) );
 UDR=c;
}

//�Ӵ��ڶ���
unsigned char USART_Receive( void )
{
   //�ȴ����ܱ�־
   while ( !(UCSRA & (1<<RXC)) )
   ;
   //����������
   return UDR;
}

//�򴮿�д�ַ���
void put_s(unsigned char *ptr)
{
 while (*ptr)
 {
  put_c(*ptr++);
 }
 put_c(0x0D);//�س�
 put_c(0x0A);//����  
}

//������BCD����16����
void dip_c(unsigned char c)
{
    unsigned char t;
 t = c/10;
 put_c(t+0x30);
 t = c%10;
 put_c(t+0x30);
    
}

//��ʼ��timer0
void timer0_init(void)
{
	TCCR0 = 0x05;
	TCNT0 = 0x4c; //��ʱʱ��50ms
	TIMSK = _BV(OCIE1A);
}

//50ms�ж�
SIGNAL(SIG_OVERFLOW0)
{

	TCNT0 = 0x4c;

	if(++i == 20)
	{
		i = 0; 
		if (++sec == 60)  
		{
 			sec = 0;
 			if (++min == 60) 
 			{
			     min = 0;
     if (++hour == 24)  
  {   
       hour = 0;
     }
   }
 }
 put_c(0x0d);
 dip_c(hour);
    put_c(':');
 dip_c(min);
 put_c(':');
 dip_c(sec);
 }

}

//USART ��ʼ��
void init_USART(void)
{
    //USART 9600 8, n,1  PC��λ�����(�����ն˵�)ҲҪ���ͬ�������ò���ͨѶ
    UCSRC = (1<<URSEL) | 0x06;
    
	//�첽��8λ���ݣ�����żУ�飬һ��ֹͣλ���ޱ���   
    //U2X=0ʱ�Ĺ�ʽ����
    UBRRL= (F_CPU/BAUDRATE/16-1)%256;
    UBRRH= (F_CPU/BAUDRATE/16-1)/256;

    UCSRA = 0x00;
    //UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	//ʹ�ܽ����жϣ�ʹ�ܽ��գ�ʹ�ܷ���
	UCSRB = (1<<RXEN)|(1<<TXEN);

}

void debug(char *str)
{
	put_s(str);
	//timer0_init();
	//sei();
}

