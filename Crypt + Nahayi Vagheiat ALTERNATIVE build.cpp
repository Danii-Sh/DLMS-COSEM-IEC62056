#include <mega328.h>
#include <delay.h>



void OBIS();
void PacketAnalyze();
void EncryptedResponse();

char Data[40];
int i,j,x,object;
int  OBIS_A, OBIS_B, OBIS_C, OBIS_D, OBIS_E, OBIS_F,choice;
int AttMeth;
char result[40];  /// Value khande shode baraye ersal dar in zakhire mishavad

int success,ValType,SendCounter,resultCounter;

///Object Related Data Types
int tempValue;                                   ///Temperature Value
char LDN[6]={83,72,68,83,72,68};                 ///Vagheiyat char LDN[6]={83,72,68,10,20,30};
int AssociationStatus=83;


////Cryptography related Data Types and Functions


const int key[16]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int AAD=0;
const int IV[12]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};   ///araye 3*4
const int sbox[16][16]={
   {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76},
   {0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0},
   {0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15},
   {0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75},
   {0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84},
   {0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf},
   {0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8},
   {0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2},
   {0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73},
   {0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb},
   {0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79},
   {0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08},
   {0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a},
   {0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e},
   {0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf},
   {0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16}};

int crypted1[16],Hash[16],Tag[16],GHASH[16];  
int temp[16],temp1[16];
int tempkey[16],tempkey1[16];
int c1,c2,c3,c4;
int Y[16],Z[16]; 
void subbytes();
void shiftrows();
void arrayXOR();
void mixcolumns();
void ExpandKey();
int GFmul(int,int);
void GFmul128();
void AES();
void buildTag();


// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{

Data[x]=UDR0;
x++;
}



void main()
{

#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif



//////GCM AES initialization             start
c1=0;
while(c1!=12)
{ Y[c1]=IV[c1];
c1++;
}
while(c1!=15)
{ Y[c1]=0;
c1++;
}
Y[c1]=1;


c1=0;
while(c1!=16)
{temp[c1]=Y[c1];
c1++;}
AES();                                      ////////////   AES (Y0)
c1=0;
while(c1!=16)
{Tag[c1]=temp[c1];
c1++;}



c1=0;
while(c1!=16)
{temp[c1]=0;
c1++;}
AES();                                      ////////////Hash         AES(0)
c1=0;
while(c1!=16)
{Hash[c1]=temp[c1];
c1++;}

//////GCM AES initialization     end



/////  DLMS/COSEM start

DDRB=0xff;

tempValue=30;


// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART0 Mode: Asynchronous
// USART Baud Rate: 9600
UCSR0A=0x00;
UCSR0B=0x98;
UCSR0C=0x06;
UBRR0H=0x00;
UBRR0L=0x67;





////ADC Configuration
ADMUX=0xe0;
ADCSRA=0x87;



delay_ms(500);

while(1)
{

while((UCSR0A&128)!=128)
{}
j=UDR0;
i=j;
if ((j>>4)==12)
{

j=i&0x0F;    
x=0; 
   if (j==0)   ///GET
   { 
   // Global enable interrupts
#asm("sei")
while(x!=12)
{}
#asm("cli")



choice=1;
}

   if (j==1)   ///SET
   {  
    // Global enable interrupts
#asm("sei")

while(x!=14)
{}
delay_ms(100);
if(x>14)
{while(x!=20)
{}
}

#asm("cli")
 


choice=2;
}

   if (j==3)  ///ACTION
   {    
   // Global enable interrupts
#asm("sei")
while(x!=12)
{}
#asm("cli")



choice=3;
}



} 

PacketAnalyze();
OBIS();
EncryptedResponse();




///PORTB=result[10];            /////  Baraye Test:::: PORTB=object;delay_ms(1000);PORTB=tempValue;delay_ms(1000);
///delay_ms(1000);


ADCSRA|=0x40;
while((ADCSRA&16)!=16)
{}                                ///// Mitavan LM35Counter Gozasht bare refreshe har chand bar yebar
tempValue=ADCH;
}

 }  

   

void PacketAnalyze()
{
x=0;
success=0;
resultCounter=0;
SendCounter=0;
object=0;
ValType=0;
///PACKET PARSE   +   EXTRACT OBIS CODE FROM PACKET

if(choice==1)     ////get
{
OBIS_A=Data[4];
OBIS_B=Data[5];
OBIS_C=Data[6];
OBIS_D=Data[7];
OBIS_E=Data[8];
OBIS_F=Data[9];

AttMeth=Data[10];

}


if(choice==2)     ////set
{
OBIS_A=Data[4];
OBIS_B=Data[5];
OBIS_C=Data[6];
OBIS_D=Data[7];
OBIS_E=Data[8];
OBIS_F=Data[9];

AttMeth=Data[10];

if(Data[25]==15)
{
result[0]=Data[13];
resultCounter=1;

}
if(Data[12]==9)
{
resultCounter=Data[13];
x=0;
while(x!=resultCounter)
{result[x]=Data[14+x];
x++;
}}

if(Data[12]==22)
{
result[0]=Data[13];
resultCounter=1;
}
}

if(choice==3)     ////action
{
OBIS_A=Data[4];
OBIS_B=Data[5];
OBIS_C=Data[6];
OBIS_D=Data[7];
OBIS_E=Data[8];
OBIS_F=Data[9];

AttMeth=Data[10];
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
if(resultCounter==1)
{
tempValue=result[0];
success=1;
}}
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

if(choice==1)       //get
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
result[0]=LDN[0];
result[1]=LDN[1];
result[2]=LDN[2];     //// Esme dastgah  "   SHD102030  " ast           ke haman meghdar  (( LDN )) ast
result[3]=LDN[3];
result[4]=LDN[4];
result[5]=LDN[5];
resultCounter=6;
ValType=9;
success=1;
}
}
if(choice==2)         ///set
{
if(resultCounter==6)
{
LDN[0]=result[0];
LDN[1]=result[1];
LDN[2]=result[2];   
LDN[3]=result[3];
LDN[4]=result[4];
LDN[5]=result[5];
success=1;
}
}
if(choice==3)      ///No Action
{}


}


if(object==195)
{

if(choice==1)        ///get
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
result[0]=1;
result[1]=0;
result[2]=3;           /// Taghribi :: Octer String { ClassId+ Version}
result[3]=0;
result[4]=15;
result[5]=0;
resultCounter=6;
ValType=9;
success=1;
}
if(AttMeth==3)
{
result[0]=8;               //// Addresse Client
result[1]=88;               //// Addresse Server
resultCounter=2;
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
result[0]=0;
resultCounter=1;
success=1;
ValType=0;
}
if(AttMeth==8)
{
result[0]=AssociationStatus;
resultCounter=1;
ValType=22;
success=1;
}
}
if(choice==2)        ///set
{
if(AttMeth==8)
{
if(resultCounter==1)
{
AssociationStatus=result[0];
success=1;

}
}

}
if(choice==3)     ///No Action
{}
}







}



void EncryptedResponse()
{

if(choice==1)        //get
{
if(success==1)
{

if(ValType==9)
{
SendCounter=6+resultCounter;


i=0;
while(i!=resultCounter)
{
result[6+i]=result[i];
i++;
}

result[0]=196;
result[1]=1;
result[2]=193; 
result[3]=0; 
result[4]=9;
result[5]=resultCounter;
}


if(ValType!=9)
{
SendCounter=5+resultCounter;


result[5]=result[0];


result[0]=196;
result[1]=1;
result[2]=193; 
result[3]=0; 
result[4]= ValType ;

}


}
if(success==0)
{
result[0]=196;
result[1]=1; 
result[2]=193; 
result[3]=1; 
result[4]=250;

SendCounter=5;

} }

if(choice==2)        //set
{
result[0]=197;
result[1]=1;
result[2]=193; 

if(success==1)
{result[3]=0;}
if(success==0)
{result[3]=250;}


SendCounter=4; 
}

if(choice==3)       ///action
{  
result[0]=199;
result[1]=1; 
result[2]=193; 


if(success==1)
{result[3]=0;}
if(success==0)
{result[3]=250;}

result[4]=0;

SendCounter=5; 

}

i=SendCounter;
while(i!=16)
{
result[i]=0;
i++;
}


 
Y[15]++;       
c1=0;
while(c1!=16)
{temp[c1]=Y[c1];
c1++;}
AES();
                                      ////////////2222222222
c1=0;
while(c1!=16)
{crypted1[c1]=(temp[c1]^result[c1]);
c1++;}

buildTag();

result[0]=Y[15];
  

i=1;
while(i!=17)
{
result[i]=crypted1[i-1];
i++;
}


while(i!=33)
{
result[i]=Tag[i-17];
i++;
}

i=0;
while(i!=33)
{
UDR0=result[i];
while((UCSR0A&32)!=32)
{}
i++;
}



} 






 ////Cryptography  Functions
void AES()
{

c1=0;
while(c1!=16)
{tempkey[c1]=key[c1];
c1++;}


c3=0;
while(c3<10)
{
arrayXOR();

subbytes();

shiftrows();
if(c3!=9)
{
mixcolumns();
}
ExpandKey();

if(c3==9)
{arrayXOR();}
c3++;
}


}


 
 
void subbytes()
{
int x,y;

c1=0;
while(c1!=16)
{

x=temp[c1]>>4;
y=(temp[c1] & 15);

temp[c1]= sbox[x][y];
c1++;
}
}


void arrayXOR()
{
c1=0;
while(c1!=16)
{
temp[c1]=temp[c1]^tempkey[c1];
c1++;
}
}


void shiftrows()
{
c1=0;
while(c1!=16)
{temp1[c1]=temp[c1];
c1++;}

temp[13]=temp1[1];
temp[9]=temp1[13];
temp[5]=temp1[9];       ////2nd row
temp[1]=temp1[5];

temp[10]=temp1[2];
temp[6]=temp1[14];   
temp[14]=temp1[6];      ////3d row
temp[2]=temp1[10];

temp[7]=temp1[3];
temp[11]=temp1[7];   
temp[15]=temp1[11];     ////4d row
temp[3]=temp1[15];


}

void mixcolumns()
{
c1=0;

while(c1<16)
{
if(c1%4==0)
{temp1[c1]=(GFmul(0x03,temp[c1+1]))^(GFmul(0x02,temp[c1]))^(temp[c1+2])^(temp[c1+3]);
}

if(c1%4==1)
{temp1[c1]=(GFmul(0x03,temp[c1+1]))^(GFmul(0x02,temp[c1]))^(temp[c1+2])^(temp[c1-1]);
}

if(c1%4==2)
{temp1[c1]=GFmul(0x03,temp[c1+1])^GFmul(0x02,temp[c1])^(temp[c1-1])^(temp[c1-2]);
}

if(c1%4==3)
{temp1[c1]=GFmul(0x03,temp[c1-3])^GFmul(0x02,temp[c1])^(temp[c1-2])^(temp[c1-1]);
}
c1++;
}
c1=0;
while(c1!=16)
{temp[c1]=temp1[c1];
c1++;}

}

int GFmul(int x,int y)
{
int z=0;
int w=0;
c4=0;
while(c4<8)
{
w=y<<c4;
if((x &(1<<c4))>0)
{
if(w>255)
{w=w^0x1b;
w=w & 255;
}
z= w ^ z;
}
c4++;
}
return z;
}

void ExpandKey()
{
int x,y;

tempkey1[3]=tempkey[12];
tempkey1[0]=tempkey[13];
tempkey1[1]=tempkey[14];
tempkey1[2]=tempkey[15];





c1=0;
while(c1<4)
{
x=tempkey1[c1]>>4;
y=(tempkey1[c1] & 15);

tempkey1[c1]= sbox[x][y];

c1++;
}


switch(c3){
case 0 :tempkey1[0]=tempkey1[0]^0x01; break;
case 1 :tempkey1[0]=tempkey1[0]^0x02; break;
case 2 :tempkey1[0]=tempkey1[0]^0x04; break;
case 3 :tempkey1[0]=tempkey1[0]^0x08; break;
case 4 :tempkey1[0]=tempkey1[0]^0x10; break;
case 5 :tempkey1[0]=tempkey1[0]^0x20; break;
case 6 :tempkey1[0]=tempkey1[0]^0x40; break;
case 7 :tempkey1[0]=tempkey1[0]^0x80; break;
case 8 :tempkey1[0]=tempkey1[0]^0x1b; break;
case 9 :tempkey1[0]=tempkey1[0]^0x36; break; }







c1=0;
while(c1<4)
{
tempkey1[c1]=tempkey[c1]^tempkey1[c1];
c1++;}

while(c1<16)
{
tempkey1[c1]=tempkey1[c1-4]^tempkey[c1];
c1++;}

c1=0;
while(c1<16)
{
tempkey[c1]=tempkey1[c1];
c1++;}
}   


void GFmul128()
{

c2=0;
while(c2<16)
{
GHASH[c2]=0;
c2++;}


c2=0;
while (c2!=128) 
{
 if (((Hash[c2>>3] >> (7-(c2 & 0x07 )))&1) ==1) 
{ 
c3=0;
while(c3!=16)
{GHASH[c3]=GHASH[c3]^temp[c3];
c3++;} 
}
if((temp[15]& 1)==0)
{
c3=0;
while(c3!=16)
{
Z[c3]=temp[c3] & 1;
c3++;
}
c3=1;
while(c3!=16)
{
temp[c3]=(temp[c3]>>1)|(Z[c3-1]<<7);
c3++;
}
temp[0]=temp[0]>>1;
}

else
{
c3=0;
while(c3!=16)
{
Z[c3]=temp[c3] & 1;
c3++;
}
c3=1;
while(c3!=16)
{
temp[c3]=(temp[c3]>>1)|(Z[c3-1]<<7);
c3++;
}
temp[0]=temp[0]>>1;
temp[0]=temp[0]^0xe1;
}

c2++;
}}


void buildTag()
{


c1=0;
while(c1!=16)
{temp[c1]=(crypted1[c1]);
c1++;}
        GFmul128(); 

c1=0;
while(c1!=16)
{temp[c1]=(GHASH[c1]);
c1++;}

temp[15]=temp[15]^0x80;

        GFmul128();  
c1=0;
while(c1!=16)
{Tag[c1]=Tag[c1]^GHASH[c1];                    ////Tag = Y0 ,,,   H = AES(0)
c1++;}


}