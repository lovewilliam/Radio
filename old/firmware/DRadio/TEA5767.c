#include "TEA5767.h"

#include "../debug.h"

//TWINT ��־���������ͨ�����д "1" �����
unsigned char set5767(unsigned char *dat) 
{ 
        uchar i = 0; 
         
        TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // SEND START SIGNAL 
        while (!(TWCR & (1<<TWINT)));           // WAIT FOR START SIG �ȴ�START�źŷ������

        if ((TWSR & 0xF8) != START)
		{
			return 0;
		}
        TWDR = SLA_W;                            // send address дTEA5767�ĵ�ַ
        TWCR = (1<<TWINT)|(1<<TWEN);             //��TWI�жϱ�־��ʹ�ܷ��͡�����
        while (!(TWCR & (1<<TWINT)));            //�ȴ�������ɡ�����

        if ((TWSR & 0xF8) !=MT_SLA_ACK)
		{
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL
			return 0;  //ACK
		}
        for ( i = 0; i < 5; i++ ) 
        { 
               
            TWDR = dat[i];                  //д����
            TWCR = (1<<TWINT)|(1<<TWEN);         // send data 
            while (!(TWCR & (1<<TWINT)));        //�ȴ��������
			if ((TWSR & 0xF8) != MT_DATA_ACK)
			{
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL
				return 0;//ACK
			}
        }
       
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL 
        return 1;
} 

void read5767(void)
{
	uchar i = 0;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // SEND START SIGNAL
	while (!(TWCR & (1<<TWINT)));           // WAIT FOR START SIG
	//if ((TWSR & 0xF8) != START)  ERROR();
	
	TWDR = SLA_R;                            // send address
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//if ((TWSR & 0xF8) !=MT_SLA_ACK) ERROR();
	//TWCR = (1<<TWINT) | (1<<TWEN);
	
	for ( i = 0; i < 5; i++ )
	{
		TWCR = (1<<TWINT) | (1<<TWEN);      // read data
		while (!(TWCR & (1<<TWINT)));
		//readdata[i] = TWDR ;//save�汾û��[i]
		
		debug("rd:");
		dip_c(TWDR);
		debug("\r\n");
	//	if ((TWSR & 0xF8) != MT_DATA_ACK) ERROR();
	}
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL

	//LCD_write_english_string(10,3,"rOK");
}




///////////////////////////////////////////////////////////////////////////////////////
unsigned char TEA5767_INTI(void)
{
	unsigned char senddata[5];

	TWBR = 12*12;         //��Ƶϵ�� SCL frequency = CPU Clock frequency/(16+2*(TWBR)*4)
	                   //Լ 70KHz ��ʱ��Ƶ��
	TWCR = (1<<TWEN);  //SEND STOP SIGNAL

	senddata[0] = 0x29;  //load 88.3MHz BIT6����ѡ���Ƿ���̨ģʽ1��/0�� 0x29
	                     //BIT7 MUTE1/NO MUTE0    PLL13...8
	senddata[1] = 0xF5;  // PLL7...0   F1
	senddata[2] = 0x60;  //bit7 ����ѡ������1/������̨0  ADCѡ�� 0x20 ADC 5
	                                                           //0x40 ADC 7
															   //0x60 ADC 10
	senddata[3] = 0x12;  //bit5 ����ѡ���ձ�1/ŷ��ģʽ0
	senddata[4] = 0x00;  //���ø�
	return (set5767(senddata));
}

/////////////////////////////////////////////////////////////////////////////////////////////
//�ֶ�����Ƶ��
void set_frequency(unsigned long frequency) //�趨Ƶ��KHz
{
	unsigned char senddata[5];
    unsigned int PLL;
	unsigned char falg;
    if(senddata[2]&0x10)
		PLL=(unsigned int)((float)((frequency+225)*4)/(float)32.768);
    else
		PLL=(unsigned int)((float)((frequency-225)*4)/(float)32.768);

    senddata[0] = PLL/256;
	senddata[1] = PLL%256;
	senddata[0]&= 0x3F;
	senddata[2] = 0x00;
	senddata[3] = 0x12;
	senddata[4] = 0x00;
    falg=set5767(senddata);
}


