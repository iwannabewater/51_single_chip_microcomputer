#include <reg52.h> 
#define uint unsigned int
#define uchar unsigned char   	//�궨��
#define LCD1602 P0
sbit SET=P1^0;    			//���������
sbit DEC=P1^1;    			//������ټ�
sbit ADD=P1^2;    			//�������Ӽ�
sbit BUZZ=P2^4;    			//���������
sbit ALAM=P2^3;					//����̵���
sbit ALAM1=P2^2;
sbit DQ=P2^1;     			//����DS18B20����I/O	
sbit RS = P2^5;
sbit EN = P2^7;
bit shanshuo_st;    			//��˸�����־
bit beep_st;     				//�����������־
uchar x=0;      				//������

uchar code tab1[]={"Now Tem:   .  C "};
uchar code tab2[]={"TH:   C  TL:   C"};
uint c;
uchar Mode=0;     			//״̬��־
signed char TH=40;  		//���ޱ����¶ȣ�Ĭ��ֵΪ40
signed char TL=10;   		//���ޱ����¶ȣ�Ĭ��ֵΪ10
//============================================================================================
//====================================DS18B20=================================================
//============================================================================================
/*****��ʱ�ӳ���*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
void delay(uint xms)//��ʱ�������вκ���
{
	uint x,y;
	for(x=xms;x>0;x--)
	 for(y=110;y>0;y--);
}
/*****��ʼ��DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ��λ
  Delay_DS18B20(8);    //������ʱ
  DQ = 0;         //��Ƭ����DQ����
  Delay_DS18B20(80);   //��ȷ��ʱ������480us
  DQ = 1;         //��������
  Delay_DS18B20(14);
  x = DQ;           //������ʱ�����x=0���ʼ���ɹ���x=1���ʼ��ʧ��
  Delay_DS18B20(20);
}
/*****��һ���ֽ�*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****дһ���ֽ�*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
/*****��ȡ�¶�*****/
unsigned int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0x44);  //�����¶�ת��
  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
  a=ReadOneChar();     //����8λ
  b=ReadOneChar();    //����8λ
  t=b;
  t<<=8;
  t=t|a;
  tt=t*0.0625;
 // t= tt*10+0.5;     //�Ŵ�10���������������
  t= tt*10+0.5; 
  return(t);
}

/*****��ȡ�¶�*****/
void check_wendu(void)
{
	c=ReadTemperature()-5;  			//��ȡ�¶�ֵ����ȥDS18B20����Ư���
	if(c>1200)
	c=1200;
}

/********Һ��д��ָ�����д�����ݺ������Ժ�ɵ���**************/

void write_1602com(uchar com)//****Һ��д��ָ���****
{
	RS=0;//����/ָ��ѡ����Ϊָ��
//	rw=0; //��дѡ����Ϊд
	LCD1602=com;//��������
	delay(1);
	EN=1;//����ʹ�ܶˣ�Ϊ������Ч���½�����׼��
	delay(1);
	EN=0;//en�ɸ߱�ͣ������½��أ�Һ��ִ������
}


void write_1602dat(uchar dat)//***Һ��д�����ݺ���****
{
	RS=1;//����/ָ��ѡ����Ϊ����
//	rw=0; //��дѡ����Ϊд
	LCD1602=dat;//��������
	delay(1);
	EN=1; //en�øߵ�ƽ��Ϊ�����½�����׼��
	delay(1);
	EN=0; //en�ɸ߱�ͣ������½��أ�Һ��ִ������
}


void lcd_init()//***Һ����ʼ������****
{
	uchar a;
	write_1602com(0x38);//����Һ������ģʽ����˼��16*2����ʾ��5*7����8λ����
	write_1602com(0x0c);//����ʾ����ʾ���
	write_1602com(0x06);//�������ƶ�������Զ�����
	write_1602com(0x01);//����ʾ

	write_1602com(0x80);//������ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
	for(a=0;a<16;a++)
	{
		write_1602dat(tab1[a]);//��Һ����д������ʾ�Ĺ̶����Ų���
		delay(3);
	}
	write_1602com(0x80+0x40);//ʱ����ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
	for(a=0;a<16;a++)
	{
		write_1602dat(tab2[a]);//д��ʾʱ��̶����ţ�����ð��
		delay(3);
	}

}

void display()
{
	if(Mode==0)
	{
		write_1602com(0x80+8);
		write_1602dat(c/1000+0x30);
		write_1602dat((c%1000)/100+0x30);
		write_1602dat(((c%1000)%100)/10+0x30);
		write_1602com(0x80+12);
		write_1602dat(((c%1000)%100)%10+0x30);
		write_1602com(0x80+13);
		write_1602dat(0xdf);
		write_1602com(0x80+0x40+3);
		write_1602dat(TH/10+0x30);
		write_1602dat(TH%10+0x30);
		write_1602dat(0xdf);
		write_1602com(0x80+0x40+12);
		write_1602dat(TL/10+0x30);
		write_1602dat(TL%10+0x30);
		write_1602dat(0xdf);			
	}								  
}
//=====================================================================================

/*****��ʼ����ʱ��0*****/
void InitTimer(void)
{
	TMOD=0x1;
	TH0=0x3c;
	TL0=0xb0;     //50ms������12M��
	EA=1;      //ȫ���жϿ���
	TR0=1;
	ET0=1;      //������ʱ��0
}

void KEY()
{
			//���ܼ�
	if(SET==0)
	{
		BUZZ=0;
		delay(10);
		if(SET==0)
		{
			Mode++;
			if(Mode==3)
			Mode=0;
			BUZZ=1;
		}
		while(SET==0)
		{
			if(Mode==0)
				{
				//	write_1602com(0x80+0x40+6);
					write_1602com(0x0c);
				}	
			else if(Mode==1)
				{
					write_1602com(0x80+0x40+4);
					write_1602com(0x0f);
				}	
			else
				{
					write_1602com(0x80+0x40+13);
					write_1602com(0x0f);
				}							
		}
	}
	//����
	if(ADD==0&&Mode==1)
	{
		BUZZ=0;
		delay(10);
		if(ADD==0)	
		{
			TH++;
			if(TH>=99)	
			TH=99;
			write_1602com(0x80+0x40+3);
			write_1602dat(TH/10+0x30);
			write_1602dat(TH%10+0x30);
			write_1602com(0x80+0x40+4);	
			BUZZ=1;
		}
		while(ADD==0);
		
	}
	//����
	if(DEC==0&&Mode==1)
	{
		BUZZ=0;
		delay(10);
		if(DEC==0)
		{
			TH--;
			if(TH==TL)	
			TH=TL+1;
			write_1602com(0x80+0x40+3);
			write_1602dat(TH/10+0x30);
			write_1602dat(TH%10+0x30);
			write_1602com(0x80+0x40+4);	
			BUZZ=1;
		}
		while(DEC==0);
	}
	if(ADD==0&&Mode==2)
	{
		BUZZ=0;
		delay(10);
		if(ADD==0)	
		{
			TL++;
			if(TL==TH)	
			TL=TH-1;
			write_1602com(0x80+0x40+12);
			write_1602dat(TL/10+0x30);
			write_1602dat(TL%10+0x30);
			write_1602com(0x80+0x40+13);	
			BUZZ=1;
		}
		while(ADD==0);
		
	}
	//����
	if(DEC==0&&Mode==2)
	{
		BUZZ=0;
		delay(10);
		if(DEC==0)
		{
			TL--;
			if(TL<=0)	
			TL=0;
			write_1602com(0x80+0x40+12);
			write_1602dat(TL/10+0x30);
			write_1602dat(TL%10+0x30);
			write_1602com(0x80+0x40+13);	
			BUZZ=1;
		}
		while(DEC==0);		
	}
}

/*****�����ӳ���*****/
void Alarm()
{
	if(x>=10){beep_st=~beep_st;x=0;}
	if(Mode==0)
	{
		if((c/10)>=TH)
		{
			ALAM=0;
			ALAM1=1;
			if(beep_st==1)
			BUZZ=0;
			else
			BUZZ=1;
		}
		else if((c/10)<TL)
		{
			ALAM1=0;
			ALAM=1;
			if(beep_st==1)
			BUZZ=0;
			else
			BUZZ=1;
		}
		else
		{
			BUZZ=1;
			ALAM=1;
			ALAM1=1;		
		}
	}
	else
	{
		BUZZ=1;
		ALAM=1;
		ALAM1=1;
	}
}

/*****������*****/
void main(void)
{
	uint z;
	delay(1);
	lcd_init();
	delay(1);
	InitTimer();    //��ʼ����ʱ��
	
	for(z=0;z<100;z++)
	{
		check_wendu();
		delay(1);        
	} 
	while(1)
	{
		
		display();
		KEY();
		Alarm(); 
		check_wendu();
	}
}

/*****��ʱ��0�жϷ������*****/
void timer0(void) interrupt 1
{
 TH0=0x3c;
 TL0=0xb0;
 x++;
}