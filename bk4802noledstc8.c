#include "STC8F.h"
#include "intrins.h"
#include "IICANDDELAY.h"
#include "codetab.h"
sbit set=P3^7;//must connect not more than 8.2K resistor to GND
//MCU and BK4802N share 3.3v VCC(STC8MCU must using 3.3v VCC)
//BK4802N pin19-pin22 must connect to 3.3v.
sbit ppt=P5^4;//ppt must be connected to BK4802N pin23.
sbit test=P5^5;//test(p5.5) connect a LED to GND to indict 4802n IIC RUN.
//DEFAULT FERQ:TX=RX=439.725mh.uSING "BK4802NCalc.exe to calculate.
int rxfreq[]={
0x52BE,0xF850,0x0000};
int txfreq[]={
0x52C5,0x925F,0x0000};
char txinf[]="TX...";
char rxinf[]="RX...";
char blankinf[]="        ";
char newtxfreq[6];
char newrxfreq[6];
unsigned char newtxfreqinf[6];
//char newrxfreqinf[6];
int rxreg[]={ //rx setting for register
0x0300,
0x8e04,
0xF140,
0xED00,
0x17E0,
0xe0e0,
0x8543,
0x0700,
0xA066,
0xFFFF,
0xFFE0,
0x07a0,
0x9E3C,
0x1F00,
0xD1D1,
0x200F,
0x01FF,
0xE000,
0x0340
 };
 int txreg[]={//tx setting for register
0x7C00,
0x0c04,
0xF140,
0xED00,
0x3fE0,
0xe0e0,
0x8543,
0x0700,
0xA066,
0xFFFF,
0xffe0,
0x061f,
0x9e3c,
0x1f00,
0xd1C1,
0x200f,
0x01FF,
0xE000,
0x0c00
};

void setchannel(char k)//setting new trx freq from EEPROM in MCU
	 {
		 
		 int i;
		 
		 	for(i=0;i<6;i++)
			 {
				newrxfreq[i]=IapRead(i+6+k*32);
			  newtxfreq[i]=IapRead(i+k*32);
			 }
	 }
void setchannelinf(char k)//setting new trx freq information display on OLED  from EEPROM in MCU
	 {
		 
		 int i;
		 
		 	for(i=0;i<6;i++)
			 {
				newtxfreqinf[i]=IapRead(i+16+k*32);
			 }
	 }
void main()
{
	unsigned char i,f;// i do for 'for',f do for store channel No.
	unsigned char k=0;//k is do for trx 
	//Start 
	test=1;
  Delay50ms();
	OLED_Init(); //OLED初始化
			for(i=0; i<8; i++)//通过点整显示汉字 -- i表示字表数组的位置
		{
			OLED_P16x16Ch(i*16,0,i);//In the name of the Moon ,I will punish you.
		 	OLED_P16x16Ch(i*16,2,i+8); 	//Moon promise power, make up!
		}
		OLED_P8x16Str(0,6,"BG7QKU");
		delay(10000);
		OLED_CLS();//清屏
		test=0;
		OLED_P8x16Str(0,0,"BG7QKU");
		for(i=0; i<8; i++)
		 {
			 OLED_P16x16Ch(i*16,6,i);
		 }
		writing(0x90,23,0xa8,0XD0);//Init rx reg23.
		if(P11==0)//if default or not?
		{
			for(i=4;i<=22;i++)
			{
			 writing(0x90,i,HIGHBYTE(rxreg[i-4]),LOWBYTE(rxreg[i-4]));//init default reg. ro rx
			}
			for(i=5;i>2;i--)
			{
				writing(0x90,i-3,HIGHBYTE(rxfreq[i-3]),LOWBYTE(rxfreq[i-3]));//init default reg. to rx freq
			}
			OLED_P8x16Str(0,4,"439.725");
			OLED_P8x16Str(0,2,rxinf);
		}
		
		else
		{
			f=IapRead(0x0200);//read store channel No.
			if (f>3)
				f=0;
			setchannel(f);
			setchannelinf(f);      
			OLED_P8x16Str(0,4,newtxfreqinf);
			k=1;
		}
			
		test=1;	
		while(1)
		{
			if(set==1)//teat channel change key
			{
				Delay30ms();//kill swim
				if(set==1)
				{
				 Delay30ms();//down to LOW to set channel
				if(set==0)
				{
					f++;
					if(f>3)
						f=0;
					IapErase(0x0200);//erase EEPROM before write
					IapProgram(0x0200,f);//write EEPORM
					setchannel(f);
				  setchannelinf(f);
				  OLED_P8x16Str(0,4,newtxfreqinf);
					
			    k=1;
				}
			 }
			}
			//Only using default freq
			if(ppt==1&&k==0&&P11==0)//Set to TX if PTT =HIGH.
			{
			  test=0;
				Delay30ms();
				if(ppt==1)
				{
					Delay30ms();
					  for(i=4;i<=22;i++)
	        {
				   writing(0x90,i,HIGHBYTE(txreg[i-4]),LOWBYTE(txreg[i-4]));
			    }
			
		      for(i=5;i>2;i--)
		      {
			     writing(0x90,i-3,HIGHBYTE(txfreq[i-3]),LOWBYTE(txfreq[i-3]));
		      }
					Delay30ms();
					OLED_P8x16Str(0,2,blankinf);
					OLED_P8x16Str(0,2,"439.725");
					test=1;
					k=1;
				}
			}
				if(ppt==0&&k==1&&P11==0)//Set to RX.
				{
					test=0;
					Delay30ms();
				if(ppt==0)
				{
					Delay30ms();
					writing(0x90,23,0xa8,0XD0);
					  for(i=4;i<=22;i++)
	       {
				   writing(0x90,i,HIGHBYTE(rxreg[i-4]),LOWBYTE(rxreg[i-4]));
			    }
			
		      for(i=5;i>2;i--)
		      {
			     writing(0x90,i-3,HIGHBYTE(rxfreq[i-3]),LOWBYTE(rxfreq[i-3]));
		      }
					Delay30ms();
					OLED_P8x16Str(0,2,blankinf);
					OLED_P8x16Str(0,2,"439.725");
		      test=1;
					k=0;
			  }
		   }
			//Using pre-set freq
			if(ppt==1&&k==0&&P11==1)//Set to TX if PTT =HIGH.
			{
        test=0;
				Delay30ms();
				if(ppt==1)
				{
					setchannel(f);
				  setchannelinf(f);
					test=0;
					OLED_P8x16Str(0,4,newtxfreqinf);
					Delay30ms();
					  for(i=4;i<=22;i++)
	       {
				   writing(0x90,i,HIGHBYTE(txreg[i-4]),LOWBYTE(txreg[i-4]));
			    }
			
		      for(i=5;i>2;i--)
		      {
			     writing(0x90,i-3,newtxfreq[2*(i-3)],newtxfreq[2*(i-3)+1]);
		      }
					Delay30ms();
					OLED_P8x16Str(0,2,blankinf);
					OLED_P8x16Str(0,2,txinf);
					test=1;
					k=1;
				}
			}
				if(ppt==0&&k==1&&P11==1)//Set to RX.
				{

					Delay30ms();
				if(ppt==0)
				{

					test=0;

					Delay30ms();
					writing(0x90,23,0xa8,0XD0);
					  for(i=4;i<=22;i++)
	       {
				   writing(0x90,i,HIGHBYTE(rxreg[i-4]),LOWBYTE(rxreg[i-4]));
			    }
			
		      for(i=5;i>2;i--)
		      {
			     writing(0x90,i-3,newrxfreq[2*(i-3)],newrxfreq[2*(i-3)+1]);
		      }
					Delay30ms();
					OLED_P8x16Str(0,2,blankinf);
					OLED_P8x16Str(0,2,rxinf);
					test=1;
					k=0;
			  }
		   } 
		}
}