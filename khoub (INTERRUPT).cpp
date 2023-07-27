#include <mega32.h>
#include <delay.h>
#include <lcd.h>
#include <ctype.h>

#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm   


void OBIS();
void PacketAnalyze();
void Response();

char Data[40];
int i,j,x,object;
int  OBIS_A, OBIS_B, OBIS_C, OBIS_D, OBIS_E, OBIS_F,choice;
int AttMeth;
char result[40];  /// Value khande shode baraye ersal dar in zakhire mishavad
int resultCounter;
int success,ValType,SendCounter;
int temp;
///Object Related Data Types
int tempValue;



// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{

Data[x]=UDR;
x++;
}
















void main()
{
DDRB=0xff;

tempValue=10;


// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 110
UCSRA=0x00;
UCSRB=0x98;
UCSRC=0x86;
UBRRH=0x02;
UBRRL=0x37;



lcd_init(16);
lcd_puts("DLMS");
delay_ms(500);

while(1)
{

while(UCSRA.7!=1)
{}


if ((UDR)==67)
{


  while(UCSRA.7!=1)
  {}
j=UDR; 
x=0; 
   if (j==48)   ///GET
   { 
   // Global enable interrupts
#asm("sei")
while(x!=24)
{}
#asm("cli")

lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Get");
delay_ms(500);
choice=1;
}

   if (j==49)   ///SET
   {  
    // Global enable interrupts
#asm("sei")
while(x!=28)
{}
#asm("cli")
 
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Set");
delay_ms(500);
choice=2;
}

   if (j==51)  ///ACTION
   { 
   // Global enable interrupts
#asm("sei")
while(x!=24)
{}
#asm("cli")

lcd_clear();
lcd_gotoxy(0,0);
lcd_puts("Action");
delay_ms(500);
choice=3;
}



} 

PacketAnalyze();
OBIS();
Response();




PORTB=result[0];            /////  Baraye Test:::: PORTB=object;delay_ms(1000);PORTB=tempValue;delay_ms(1000);
        

delay_ms(1000);
 }




 }  

   

void PacketAnalyze()
{
x=0;
success=0;
resultCounter=0;
SendCounter=0;
object=0;
///PACKET PARSE   +   EXTRACT OBIS CODE FROM PACKET

if(choice==1)     ////get
{
OBIS_A=(16*toint(Data[8]))+toint(Data[9]);
OBIS_B=(16*toint(Data[10]))+toint(Data[11]);
OBIS_C=(16*toint(Data[12]))+toint(Data[13]);
OBIS_D=(16*toint(Data[14]))+toint(Data[15]);
OBIS_E=(16*toint(Data[16]))+toint(Data[17]);
OBIS_F=(16*toint(Data[18]))+toint(Data[19]);

AttMeth=(16*toint(Data[20]))+toint(Data[21]);

}


if(choice==2)     ////set
{
OBIS_A=(16*toint(Data[8]))+toint(Data[9]);
OBIS_B=(16*toint(Data[10]))+toint(Data[11]);
OBIS_C=(16*toint(Data[12]))+toint(Data[13]);
OBIS_D=(16*toint(Data[14]))+toint(Data[15]);
OBIS_E=(16*toint(Data[16]))+toint(Data[17]);
OBIS_F=(16*toint(Data[18]))+toint(Data[19]);

AttMeth=(16*toint(Data[20]))+toint(Data[21]);

if(Data[25]==70)
{
result[0]=(10*toint(Data[26]))+toint(Data[27]);
resultCounter=1;
}
}


if(choice==3)     ////action
{
OBIS_A=(16*toint(Data[8]))+toint(Data[9]);
OBIS_B=(16*toint(Data[10]))+toint(Data[11]);
OBIS_C=(16*toint(Data[12]))+toint(Data[13]);
OBIS_D=(16*toint(Data[14]))+toint(Data[15]);
OBIS_E=(16*toint(Data[16]))+toint(Data[17]);
OBIS_F=(16*toint(Data[18]))+toint(Data[19]);

AttMeth=(16*toint(Data[20]))+toint(Data[21]);
}






/////Calculate Object Number

if(OBIS_A==0)
{
if(OBIS_B==0)
{
if(OBIS_C==42)
{if(OBIS_D==0)
{if(OBIS_E==0)
{if(OBIS_F==255)
{object=201;}}}}
if(OBIS_C==40)
{if(OBIS_D==0)
{if(OBIS_E==0)                      //////shomare Object haro az OBIS helper Bardashtam
{if(OBIS_F==255)
{object=195;}}}}
if(OBIS_C==96)
{if(OBIS_D==9)
{if(OBIS_E==0)
{if(OBIS_F==255)
{object=367;}}}}
}}








}

void OBIS()
{

if(object==367)
{

if(choice==1)        //get
{
if(AttMeth==1)
{
result[0]=0;
result[1]=0;
result[2]=96;
result[3]=9;
result[4]=0;
result[5]=255;
resultCounter=6;
ValType=9;
success=1;
}
if(AttMeth==2)
{
result[0]=tempValue;
resultCounter=1;
ValType=15;
success=1;
}
if(AttMeth==3)
{
result[0]=9;                  /////   9 dar ScalerUnit be manaye daraje santigrad ast
resultCounter=1;
ValType=22;
success=1;
}
}
if(choice==2)       ///set
{

if(AttMeth==2)
{
tempValue=result[0];
success=1;
}
}
if(choice==3)       ///action
{

if(AttMeth==1)
{
tempValue=25;
success=1;
}

}

}


if(object==201)
{

if(choice==1)
{
if(AttMeth==1)
{
result[0]=0;
result[1]=0;
result[2]=42;
result[3]=0;
result[4]=0;
result[5]=255;
resultCounter=6;
ValType=9;
success=1;
}
if(AttMeth==2)
{
result[0]=83;
result[1]=72;
result[2]=68;     //// Esme dastgah  "   SHD102030  " ast           ke haman meghdar  (( LDN )) ast
result[3]=10;
result[4]=20;
result[5]=30;
resultCounter=6;
ValType=9;
success=1;
}
}
if(choice==2)
{}
if(choice==3)      ///No Action
{}


}


if(object==195)
{

if(choice==1)
{
if(AttMeth==1)
{
result[0]=0;
result[1]=0;
result[2]=40;
result[3]=0;
result[4]=0;
result[5]=255;
resultCounter=6;
ValType=9;
success=1;
}
if(AttMeth==2)
{

}
if(AttMeth==3)
{
result[0]=8;               //// Addresse Client
result[1]=88;               //// Addresse Server
resultCounter=1;
ValType=9;
success=1;
}
if(AttMeth==4)
{
result[0]=60;
result[1]=85;
result[2]=74;       ////Tozihat Dar Notation Va IEC 53 (part  7.3.7.1)     ///LN refrencing with Ciphering
result[3]=5;
result[4]=8;
result[5]=1;
result[6]=3;
resultCounter=7;
ValType=9;
success=1;
}
if(AttMeth==5)
{
result[0]=0;
result[1]=0;
result[2]=29;             ////conformance = 00001D   //Max size Recieve=Transmit= 20 //DLMS version = 6 //
result[3]=20;
result[4]=20;
result[5]=6;
resultCounter=6;
ValType=9;
success=1;

}
if(AttMeth==6)
{
result[0]=60;
result[1]=85;
result[2]=74;       ////Tozihat Dar Notation Va IEC 53 (part  7.3.7.2)     ///Lowest Level Security
result[3]=5;
result[4]=8;
result[5]=2;
result[6]=0;
resultCounter=7;
ValType=9;
success=1;
}
if(AttMeth==7)        /// Be Khatere Lowest Level Security Khali ast
{


}
if(AttMeth==8)
{
result[0]=2;
resultCounter=1;
ValType=15;
success=1;
}
}
if(choice==2)
{}
if(choice==3)     ///No Action
{}
}







}



void Response()
{

if(choice==1)        //get
{
if(success==1)
{

if(ValType==9)
{
SendCounter=12+resultCounter;
result[10]=(resultCounter-(resultCounter%16))/16;
result[11]=resultCounter%16;                            /// Ba Iradati Baaye Namayesh Dar Virtual Terminal
while(resultCounter!=0)
{
result[12+resultCounter-1]=result[resultCounter-1];
resultCounter--;
}

result[0]='C';
result[1]='4';               
result[2]='0'; 
result[3]='1'; 
result[4]='C';
result[5]='1';
result[8]='0';
result[9]='9';
}
if(ValType!=9)
{
SendCounter=10+resultCounter+1;

temp=result[0];
//result[10]='0';   ///(temp-(temp%10)/10);  Baraye Test ba Virtual Terminal Nemikhad
//result[11]='A';     ////(temp%10);

result[0]='C';
result[1]='4';               
result[2]='0'; 
result[3]='1'; 
result[4]='C';
result[5]='1';

if(ValType==15)
{result[8]='0';
result[9]='F'; 

      /////result[10]='0';
     /////result[11]='A';      Baraye Test Ba Virtual Terminal
}
if(ValType==22)
{result[8]='1';
result[9]='6'; 

      //// result[10]='0';
     ///result[11]='9';          Baraye Test Ba Virtual Terminal

}
}
result[6]='0';
result[7]='0';

}
if(success==0)
{
result[0]='C';
result[1]='4'; 
result[2]='0'; 
result[3]='1'; 
result[4]='C';
result[5]='1';
result[6]='0';
result[7]='1';
result[8]='F';
result[9]='A';
SendCounter=10;

} }

if(choice==2)        //set
{
result[0]='C';
result[1]='5';               
result[2]='0'; 
result[3]='1'; 
result[4]='C';
result[5]='1';
if(success==1)
{result[6]='0';
result[7]='0';
}
if(success==0)
{result[6]='F';
result[7]='A';}



SendCounter=8; 
}

if(choice==3)       ///action
{
result[0]='C';
result[1]='7';               
result[2]='0'; 
result[3]='1'; 
result[4]='C';
result[5]='1';
if(success==1)
{result[6]='0';
result[7]='0';
}
if(success==0)
{result[6]='F';
result[7]='A';}

result[8]='0'; 
result[9]='0';

SendCounter=10; 

}

i=0;
while(i!=SendCounter)
{
UDR=result[i];
while(UCSRA.5!=1)
{}
i++;
}

}