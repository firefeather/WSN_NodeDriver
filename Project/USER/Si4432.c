/*si4432.c*/
#include "MySpi.h"
#include "Si4432.h"
#include "MyUart.h"
#include "MyGpio.h"
//#include  "Myhalt.h"
//#include "adc.h"

u8 Delayms(u8 num)
{
  unsigned short int n=0;
  u8 L,Num1, m=0;
  for(Num1=0;Num1<num;Num1++)
  for(n=0;n<1020;n++){L=n%2;if(L==0)m=0; else m=1;}
  return m;
}

u8 DelayUs(u8 num)
{
  u8 temp;
  for(temp=0;temp<16;temp++);
  return temp;
}

void SetChipModle(u8 mod)
{ 
  if(mod==1){SpiWriteDat8(SI4432_IO_PORT_CONFIGURATION , 0x06);}//tx switch on
  else if(mod==2) {SpiWriteDat8(SI4432_IO_PORT_CONFIGURATION , 0x05);}//rx
       else{SpiWriteDat8(SI4432_IO_PORT_CONFIGURATION , 0x07);}//off
}

void InitSi4432(void)
{
  SPI1_Init();

  MyGpioOutPutInit(GPIOD,GPIO_Pin_6);//RXON->PD6
  MyGpioOutPutInit(GPIOD,GPIO_Pin_7);//TXON->PD7
  
  MyGpioOutPutInit(GPIOB,GPIO_Pin_3);//PB3->SDN
  
  MyGpioHigh(GPIOD,GPIO_Pin_7);
  MyGpioHigh(GPIOD,GPIO_Pin_6);
  
  MyGpioHigh(GPIOB,GPIO_Pin_3);
}

void Si4432_On(void) {MyGpioLow(GPIOB,GPIO_Pin_3);Delayms(20);}
void Si4432_OFF(void){MyGpioHigh(GPIOB,GPIO_Pin_3);Delayms(3);}

void CalibrationClk(void)//0x62
{}
void TempConfig(u8 con)
{
  //set ADC reference voltage to 0.9V
  //SpiWriteDat8(0x68, 0x04);//write 0x04 to the Deltasigma ADC Tuning 2 register
  //SpiWriteDat8(0x1F, 0x03);//write 0x03 to the Clock Recovery Gearshift Override register
 
  //#define  SI4432_ADCGAIN_MASK     0x03              //
  //#define  SI4432_ADCREF_MASK      0x0C              //0x->1.2v  10 ->vdd/3  11 vdd/2
  //#define  SI4432_ADCSEL_MASK      0x70              //0->tempture sensor
  //#define  SI4432_ADCSTART         0x80              //adc sample start timer ->305us
  //#define  SI4432_ADCDONE          0x80              // start adc sample R when rerurn 1 adc finish
  SpiWriteDat8(SI4432_ADC_CONFIGURATION, 0x00);//0x0f
  
  //#define  SI4432_TEMPERATURE_SENSOR_CONTROL                  0x12
  // SI4432_TEMPERATURE_SENSOR_CONTROL               0x12
  //#define  SI4432_TSTRIM_MASK      0x0F              //xiuzheng temp value
  //#define  SI4432_ENTSTRIM         0x10              //enable trim
  //#define  SI4432_ENTSOFFS         0x20              //k temp ->.C
  //#define  SI4432_TSRANGE_MASK     0xC0              //01 ->-64 -> 192
  SpiWriteDat8(SI4432_TEMPERATURE_SENSOR_CONTROL,0x40); //0x12
}

u8 TempValue(void)
{
  char L, m=0;
  u8 ret;
  unsigned short int n=0;
  
  SpiWriteDat8(SI4432_ADC_CONFIGURATION, 0x80);//0x0f
  for(n=0;n<1000;n++)
  {
    L=n%2;
    if(L==0){m=0;}
    else {m=1;}
  }
  //#define  SI4432_ADC_VALUE                          0x11
  ret=SpiReadDat8(SI4432_ADC_VALUE);
  //temp=adc[7:0]*-70
  return ret;
}

u8 VbattValue(void)
{
  //vbat=1.7v+50mv*ADC
  u8 ret;
  ret=SpiReadDat8(SI4432_BATTERY_VOLTAGE_LEVEL);//0x1b
  ret=17+ret/2;
  return ret;
}

void SetTranPacLength(u8 Len)
{
  SpiWriteDat8(SI4432_TRANSMIT_PACKET_LENGTH,Len);//0x3e 
}

u8 GetRecPacLength(void)
{
  u8 ret;
  ret=SpiReadDat8(SI4432_RECEIVED_PACKET_LENGTH);//0x4b
  return ret;
}

u8 RssiValue(void)
{     
  /*rssi (dbm)=120-0.5*x x->reg value*/
  u8 ret;
  ret=SpiReadDat8(SI4432_RECEIVED_SIGNAL_STRENGTH_INDICATOR);//0x26
  ret=120-ret/2;
  return ret;
}

void SetTxPower(u8 Power)
{
  SpiWriteDat8(SI4432_TX_POWER, 0x07);
  //0x6d power [-1:20] step=3 [2:0] ->000[-1] 111->20db [3]->LNA SWITCH CONTROLLER 0->OPEN 
}

void ChipOnInit(void)
{
  unsigned char temp;
  //hard reset
  Si4432_OFF();
  Si4432_On(); 
  //Delayms(20);
  SpiReadDat8(SI4432_INTERRUPT_STATUS_1);//���жϱ�־λ����ж�
  SpiReadDat8(SI4432_INTERRUPT_STATUS_2);
  SpiWriteDat8(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, 0x80);//SW reset ��λsi4432���мĴ���λ 
  Delayms(1);
  SpiReadDat8(SI4432_INTERRUPT_STATUS_1);//���жϱ�־λ����ж�
  SpiReadDat8(SI4432_INTERRUPT_STATUS_2);
  SpiWriteDat8(SI4432_INTERRUPT_ENABLE_1, 0x00);//����SI4432�ж�ʧ��
  SpiWriteDat8(SI4432_INTERRUPT_ENABLE_2, 0x00);
  
  SpiReadDat8(SI4432_INTERRUPT_STATUS_1);//���жϱ�־λ����ж�
  SpiReadDat8(SI4432_INTERRUPT_STATUS_2);
  
  SpiWriteDat8(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, 0x01);  //si4432����
  Delayms(1);
  SpiWriteDat8(SI4432_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE,0x7f);//��г����Ϊ30 MHz��XTAL,Reset value = 01111111
  Delayms(1);
  
  SpiWriteDat8(SI4432_GPIO0_CONFIGURATION , 0x0a);//01010: Direct Digital Output�������
  SpiWriteDat8(SI4432_GPIO1_CONFIGURATION , 0x0a);
  SpiWriteDat8(SI4432_GPIO2_CONFIGURATION , 0x0e);//ADC Analog Inputģ������
  
  SpiWriteDat8(0x62 , 0x04);//��������/�ϵ縴λ����,��2���Ŵ����á�
  
  SpiWriteDat8(0x30, 0x8c);  // ʹ��PH+ FIFOģʽ����λ��ǰ�棬ʹ��CRCУ��			
  SpiWriteDat8(0x32, 0x00);  // ��ͷ�� //�����ù㲥��ַ�������
  SpiWriteDat8(0x33, 0x02);  // ͬ����3,2 ��ͬ����
		
  SpiWriteDat8(0x34, 32);    //����16��Nibble��Preamble
  SpiWriteDat8(0x35, 0x20);  //��Ҫ���4��nibble��Preamble
  SpiWriteDat8(0x36, 0x2d);  //ͬ����Ϊ 0x2dd4
  SpiWriteDat8(0x37, 0xd4);
  SpiWriteDat8(0x38, 0x00);//ͬ�ֲ�1
  SpiWriteDat8(0x39, 0x00);//ͬ�ֲ�0
//�ı䳤��ʱ�ı�
  SpiWriteDat8(0x3e, 6);    //�ܹ�����11���ֽڵ�����
  SpiWriteDat8(0x6d, 0x07); // ����Ϊ����ʷ���
  SpiWriteDat8(0x1d, 0x40); //ʹ�� afc
  	
  
  //1.2K bps setting
  SpiWriteDat8(0x1c, 0x1c); //
  SpiWriteDat8(0x20, 0x68); //
  SpiWriteDat8(0x21, 0x00); //
  SpiWriteDat8(0x22, 0x9d); // 
  SpiWriteDat8(0x23, 0x49); //
  SpiWriteDat8(0x24, 0x00); //
  SpiWriteDat8(0x25, 0xcc); //
	
  SpiWriteDat8(0x2a, 0x50);
  
  //rate
  SpiWriteDat8(0x6e, 0x4e); //
  SpiWriteDat8(0x6f, 0xa5); //
  
  
  SpiWriteDat8(0x70, 0x2f); //2c
  SpiWriteDat8(0x71, 0x23); // ���䲻��Ҫ CLK��FiFo �� FSKģʽ
  SpiWriteDat8(0x72, 0x30); // ƵƫΪ 30KHz
  
  
  SpiWriteDat8(0x79, 0x0);  // ����Ҫ��Ƶ
  SpiWriteDat8(0x7a, 0x0);  // ����Ҫ��Ƶ
  SpiWriteDat8(0x73, 0x0);  // û��Ƶ��ƫ��
  SpiWriteDat8(0x74, 0x0);  // û��Ƶ��ƫ��
	
  SpiWriteDat8(0x75, 0x53); // Ƶ������ ����Ƶ��
  SpiWriteDat8(0x76, 0x00); 
  SpiWriteDat8(0x77, 0x00);
}

u8 TData[10];
u8 TxDa(void)
{
  u8 cir=1,Temp=0;
  u8 s1,s2; 
  u32 ct=0;
  u8 erro=0;
  
  //SetChipModle(1);
  SpiWriteDat8(0x08,0x03);
  SpiWriteDat8(0x08,0x00);
  
  //SpiWriteDat8(0x34,40);
  SpiWriteDat8(SI4432_TRANSMIT_PACKET_LENGTH,6);//0x3e //�������ݳ���

  SpiWriteDat8(0x7F, TData[0]);
  SpiWriteDat8(0x7F, TData[1]);
  SpiWriteDat8(0x7F, TData[2]);
  SpiWriteDat8(0x7F, TData[3]);
  SpiWriteDat8(0x7F, TData[4]);
  SpiWriteDat8(0x7F, TData[5]);
  
  SpiWriteDat8(0x05, 0x04); //write 0x04 to the Interrupt Enable 1 register
  SpiWriteDat8(0x06, 0x00); //write 0x00 to the Interrupt Enable 2 register
  
  //Read interrupt status registers. It clear all pending interrupts and the nIRQ pin goes back to high.
  SpiReadDat8(0x03); //read the Interrupt Status1 register
  SpiReadDat8(0x04); //read the Interrupt Status2 register
  
  SpiWriteDat8(SI4432_TX_POWER, 0x07);//0x6d power [-1:20] step=3 [2:0] ->000[-1] 111->20db [3]->LNA SWITCH CONTROLLER 0->OPEN 
  Delayms(5);
  SpiWriteDat8(0x07, 0x09); //write 0x09 to the Operating Function Control 1 register
  //MyGpioHigh(GPIOA,GPIO_Pin_6);
  while(cir==1)
  {
     Temp=SpiReadDat8(0x03);
     Temp=Temp&0x04;
     if(Temp!=0){cir=0;}
     Delayms(1);
     ct++;
     if(ct==350){cir=0;}
  }
  //MyGpioLow(GPIOA,GPIO_Pin_6);
  if(ct==350)
  {
    SpiWriteDat8(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, 0x01);
    Delayms(5);
    erro=1;
    return erro; 
  }
  else 
  {
    s1 = SpiReadDat8(0x03); //read the Interrupt Status1 register
    s2 = SpiReadDat8(0x04); //read the Interrupt Status2 register

    SpiWriteDat8(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, 0x01);
    Delayms(5); 
    erro=0;
    return erro;
  } 
}

u8 RData[10];
u8 RxDa(void)
{
  u8 erro=0;
  u32  counter;
  u8 s1,s2;
  u8 cir=1,Temp=0;
  u8 Length,temp8,n;
  
  //MySetGpio(GPIOA,GPIO_Pin_1,1);
  MyGpioHigh(GPIOD,GPIO_Pin_1);
  //MyGpioHigh(GPIOC,GPIO_Pin_5);
  //SetChipModle(2);

  SpiWriteDat8(0x08,0x03);
  SpiWriteDat8(0x08,0x00);

  SpiWriteDat8(0x05, 0x03); //write 0x04 to the Interrupt Enable 1 register
  SpiWriteDat8(0x06, 0x00); //write 0x00 to the Interrupt Enable 2 register

  //Read interrupt status registers. It clear all pending interrupts and the nIRQ pin goes back to high.
  SpiReadDat8(0x03); //read the Interrupt Status1 register
  SpiReadDat8(0x04); //read the Interrupt Status2 register

  /*enable transmitter*/
  //The radio forms the packet and send it automatically.
  SpiWriteDat8(0x07, 0x05); //write 0x09 to the Operating Function Control 1 register
  
  SpiWriteDat8(0x05, 0x03); //write 0x04 to the Interrupt Enable 1 register
  SpiWriteDat8(0x06, 0x00); //write 0x00 to the Interrupt Enable 2 register
  //Read interrupt status registers. It clear all pending interrupts and the nIRQ pin goes back to high.
  SpiReadDat8(0x03); //read the Interrupt Status1 register
  SpiReadDat8(0x04); //read the Interrupt Status2 register
  
  /*wait for the packet rec interrupt*/
  
  Delayms(10);
  counter=0;
  while(cir==1)
  {
       Temp=SpiReadDat8(0x03);
       temp8=Temp&0x02;
       if(temp8!=0){cir=0;erro=0;}
       //else
       //{
       //    temp8=Temp&0x01;
        //   if(temp8!=0){cir=0;erro=0;}
       //}
       //while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
       //MyUart1Tran(Temp);
       Delayms(1);
       counter++;
       if(counter==500){cir=0;erro=1;}
  }
  cir=1;
  //MyGpioLow(GPIOA,GPIO_Pin_6);
  if(erro==0)
  {
      //MyGpioLow(GPIOD,GPIO_Pin_1);
      //MyGpioLow(GPIOC,GPIO_Pin_5);
      //read interrupt status registers to release the interrupt flags
      s1 = SpiReadDat8(0x03); //read the Interrupt Status1 register
      s2 = SpiReadDat8(0x04); //read the Interrupt Status2 register
      
      SpiWriteDat8(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, 0x01); 
      Length=6;
      for(temp8=0;temp8 < Length;temp8++)
      {
          RData[temp8] = SpiReadDat8(0x7F);	//read the FIFO Access register
      }
      Delayms(50);
  }
  //SetChipModle(3);
  return erro;
}

void Delay1s(u8 num)
{
   u8 n=0;
   for(n=0;n<num;n++)
   {
      Delayms(100); Delayms(100);Delayms(100);Delayms(100);Delayms(100);
   }
}

extern u8 DevData1[5];
extern u8 DevData2[5];
extern u8 DevData3[5];
extern u8 DevData4[5];
extern u8 DevData5[5];


u8 Sum=0;
u8 Case=2;
u8 T_NetId[3]={0x00,0x00,0x00};
void MyDev1(void)
{
   u8 stat,n;
   InitSi4432();
   ChipOnInit();
   TData[0]=1;TData[1]=1;TData[2]=1;TData[3]=1;TData[4]=1;TData[5]=1;
   
   
   while(1)
   {
      MyGpioHigh(GPIOD,GPIO_Pin_7);//TX
      MyGpioLow(GPIOD,GPIO_Pin_6);//RX
      //Delayms(20);
      stat=RxDa();//���ճɹ�statΪ0
      if(stat==1)
      {
         InitSi4432();
         ChipOnInit();
      } 
      else
      {
        //������յ�����Ҫ����Ϣ�򴮿ڴ�ӡ��������Ӧ���źš�
        
        if(RData[0]==0xab&&RData[1]==0xac&&RData[2]==0xad)
        Case=0;
        if(RData[0]==0xbc&&RData[1]==0xbd)
        Case=1;
          
        MyGpioLow(GPIOD,GPIO_Pin_7);//TX
        MyGpioHigh(GPIOD,GPIO_Pin_6);//RX
        switch(Case)
        {
        case 0 : 
                 for(n=0;n<6;n++)//���ڴ�ӡ������Ϣ
                 {
                   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
                   USART_SendData8(USART1,RData[n]); 
        
                  }
          
          
                 TData[0]=RData[3];TData[1]=RData[4];TData[2]=RData[5];TData[3]=T_NetId[0];TData[4]=T_NetId[1];TData[5]=T_NetId[2];
                 
                 stat=TxDa();//Ӧ���ͽ��յ��ĺ���λͬʱ�����ַ
                 if(stat==1)
                 {
                    //InitSi4432();
                    //ChipOnInit();
                  }
                 break;
          
         case 1 :   //���ڴ�ӡ��λ��Ϣ���ݰ�
                   Sum=RData[0]+RData[1]+RData[3]+RData[4]+RData[5];//����У���
                   if(RData[2]==Sum)
                   {
                    for(n=0;n<6;n++)
                    {
                    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
                    USART_SendData8(USART1,RData[n]); 
        
                     }
                    
                   TData[0]=RData[3];TData[1]=RData[4];TData[2]=1;TData[3]=1;TData[4]=1;TData[5]=1;
                 
                    stat=TxDa();//����Ӧ���źţ�ǰ��λΪ��λ��
                    if(stat==1)
                    {
                    //InitSi4432();
                    //ChipOnInit();
                     }
                    }
                    break;
        default :   break;
          
      
        }
       
      
         /*MyGpioLow(GPIOD,GPIO_Pin_5);//TX
         MyGpioHigh(GPIOD,GPIO_Pin_7);//RX
         Delayms(20);
         stat=TxDa();
         if(stat==1)
         {
            InitSi4432();
            ChipOnInit();
         }*/
      }
      //Delayms(100);
  }
}



u8 NetId[3];//�洢���յ�����������
u8 MyId[3]={0x00,0x00,0x01};//�����豸����ID
u8 CarId[2]={0x00,0x04};
u8 State=0;//��λ״̬���޳�0���г�1
u8 StateBefore=0;//�����ϴβɼ���������
int Count=0;//���ߴ����ۻ�
int Max=500;//count�����ֵ
int halttimes;
u16 CarValue=0;
extern u8 DevData5[5];

void MyDev2(void)//4
{
   u8 stat;int n;  
   InitSi4432();
   ChipOnInit();
   TData[0]=0xab;TData[1]=0xac;TData[2]=0xad;TData[3]=MyId[0];TData[4]=MyId[1];TData[5]=MyId[2];
   
   MyGpioLow(GPIOD,GPIO_Pin_7);//TX
   MyGpioHigh(GPIOD,GPIO_Pin_6);//RX
   
   while(1)
   {
      //Delayms(20);
      stat=TxDa();//�����������������Ϣ
      if(stat==1)
      {
         //InitSi4432();
         //ChipOnInit();
      }
      
      
     //���յ�Ӧ���źţ�
      stat=RxDa();
      if(stat==1)
      {
         InitSi4432();
         ChipOnInit();
      } 
     
        
      if(RData[0]==MyId[0]&&RData[1]==MyId[1]&&RData[2]==MyId[2])
        {
          
          for(n=3;n<6;n++)
           {
               NetId[n]=RData[n];//�����ն��豸����������
            }   
         
          TData[0]=0xbc;TData[1]=0xbd;TData[2]=Sum;
        
          
          while(1)//������Ϣ�ɼ�������ѭ��
          {   
              TData[3]=DevData5[0];
              TData[4]=DevData5[1];          
              TData[5]=State;  
              Sum=TData[0]+TData[1]+TData[3]+TData[4]+TData[5];
              TData[2]=Sum;
              stat=TxDa();//���ͳ�λ��Ϣ
              if(stat==1)
              {
                  //InitSi4432();
                 //ChipOnInit();
               } 
               
              //���յ�Ӧ���źţ�
              stat=RxDa();
             if(stat==1)
             {
                InitSi4432();
                ChipOnInit();
              } 
              
             if(RData[0]==DevData5[0]&&RData[1]==DevData5[1])//�յ�Ӧ���źţ�˵�����ݷ���������
               {
                  while(1)
                  {
                    //����  �ɼ�����  State״̬�仯������  ��Count��Max
                  
                  //����4*3.4s
                  
              //     for(halttimes=0;halttimes<4;halttimes++)
             //      {
             //        RTC_Config(4000); // 4000*0.85ms=3.4s 
                    
              //       SpiWriteDat8(0x07, 0x00);//�ر�SI4432��Ƶ�շ�����
                    
              //       halt();//����halt??????���ģʽ
              //                 
              //      } 
                    SpiWriteDat8(0x07, 0x00);//�ر�SI4432��Ƶ�շ�����
                    State = DevData5[2]; 
                                 
                    if(State!=StateBefore||Count==Max)//״̬�ı������ʱ�����������ѭ������������
                    {
                      StateBefore=State;
                      Count=0;
                      break;
                    }
                  }                
             
               }
              
              
             
          
          }
          
        
        }
      else{
             ;//���ٷ���ִ����������
           }
      
      
      //Delayms(500);
      /*MyGpioHigh(GPIOD,GPIO_Pin_7);//TX
      MyGpioLow(GPIOD,GPIO_Pin_6);//RX
      Delayms(20);
      stat=RxDa();   
      if(stat==1)
      {
         InitSi4432();
         ChipOnInit();
      }*/
  }
}
void MyTest(void)
{
  while(1)
  {
    Delayms(200);
    MyGpioLow(GPIOA,GPIO_Pin_6);
    Delayms(200);
    MyGpioHigh(GPIOA,GPIO_Pin_6);
  }
}


