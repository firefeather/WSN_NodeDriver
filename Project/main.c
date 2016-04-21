#include "stm8l15x.h"
#include "MyGpio.h"
#include "MyUart.h"
#include "MyExitInt.h"
#include "Si4432.h"
#include "MyDelay.h"
#include  "Myhalt.h"
#include "Sensor.h"

u8 ExitStatus,input_key=0,input_flag=0;
u16 UartLoop;

u8 DevData1[5];
u8 DevData2[5];
u8 DevData3[5];
u8 DevData4[5];
u8 DevData5[6];

u8 Data_Num;
u8 UartBuffer[6];
u8 UartFlag;
u8 comcount;
u8 StartFlag=1;
u8 SetIdFlag=0;
u8 TIM4_Count=0;
u8 TIM4_Outflag=0;
u16 TIM1_SET=0;


void Delay(uint32_t nCount){while (nCount != 0){nCount--;}}

void MyInit1(void)
{
  DevData1[0]=0;DevData1[1]=0;DevData1[2]=0;DevData1[3]=0;DevData1[4]=0;
  DevData2[0]=0;DevData2[1]=0;DevData2[2]=0;DevData2[3]=0;DevData2[4]=0;
  DevData3[0]=0;DevData3[1]=0;DevData3[2]=0;DevData3[3]=0;DevData3[4]=0;
  DevData4[0]=0;DevData4[1]=0;DevData4[2]=0;DevData4[3]=0;DevData4[4]=0;
  DevData5[0]=0;DevData5[1]=4;DevData5[2]=0;DevData5[3]=0;DevData5[4]=0;DevData5[5]=0;
  
  UartBuffer[0]=0;UartBuffer[1]=0;UartBuffer[2]=0;UartBuffer[3]=0;UartBuffer[4]=0;UartBuffer[5]=0;
  Data_Num=0;UartFlag=0;  
  ExitStatus=0;
}


static void CLK_Config(void)
{
  /* Select HSE as system clock source */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);
  /* system clock prescaler: 1*/
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSE)
  {}
}



u16 SensorDataGet(void)
{  
    u8 Car_State;
    u16 Distence=0;
    u16 value=0;
    u8 R_flag=4;
    u8 status = 0;
    u8 temp=0;
    u8 i;
   // u8 Count=0;
   // GPIO_Init(GPIOD, GPIO_Pin_5, GPIO_Mode_In_FL_IT);//R1��������
    u8 Flag_count=0;
    Sensor_Start();
    TIM1_SetCounter(0x0000);
    TIM1_Cmd(ENABLE);    
    TIM4_SetCounter(0x00);
    TIM4_Cmd(ENABLE);
   

    while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_5)==0&&TIM4_Outflag==0);
    if(TIM4_Outflag==0)
    {
        TIM4_Cmd(DISABLE);       
        TIM1_Cmd(DISABLE);//������
        value=TIM1_GetCounter();//����ʱ��1����ֵ
        //Distence=100*0.5*340*value/1000000;   
    
    }
    else
    {
         Distence=500;
         TIM4_Outflag=0; 
    
    }
      
      
      return value;
     
    
    
    
    
    
    
    
    
    
/*    
while(((BitStatus)(GPIOD->IDR & (uint8_t)GPIO_Pin_5)) == ~status)
{
    if(TIM4_Outflag)break;
  
}
for(int i = 0; i < 8; i++,status = ~status)//�Գ������7��ģ��
{
        while(((BitStatus)(GPIOD->IDR & (uint8_t)GPIO_Pin_5)) == status)
        {
          if(temp++ > 200)
          {
            R_flag++;
            break;
          }
        
        } 
}
if(R_flag>4)
{
      TIM1_Cmd(DISABLE);//������
      value=TIM1_GetCounter();
      Distence=100*0.5*340*value/1000000;    //��ʱ��һ 16��Ƶ ������65536�� 65535/1000000=65ms
}
else
{
      Distence=500;
      TIM4_Outflag=0;

}

 */   
    
  
   // return Car_State;
    
}









void main(void)
{  
    //ʱ������
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_DeInit();
  CLK_HSICmd(ENABLE);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);  //ϵͳʱ��1��Ƶ
  delay_init(16);
 // Delay(2000);
 // while(1);
  
  USART_DeInit(USART1);
  USART_Config();
  UartLoop=0;comcount=0;
  UartLoop=0;comcount=0;
  MyInit1();
  
 TIM1_Config();
 TIM1_Cmd(DISABLE); 
 TIM4_Config();
 TIM4_Cmd(DISABLE);
enableInterrupts();
//Delay(2000);
GPIO_Init(GPIOA,GPIO_Pin_6,GPIO_Mode_In_PU_No_IT);//KEY  
MyGpioOutPutInit(GPIOD,GPIO_Pin_0);//LED  green
MyGpioOutPutInit(GPIOD,GPIO_Pin_3);//LED  red
MyGpioOutPutInit(GPIOB,GPIO_Pin_0);//LED  flag
GPIO_SetBits(GPIOB, GPIO_Pin_0);
GPIO_SetBits(GPIOD, GPIO_Pin_0); 
GPIO_SetBits(GPIOD, GPIO_Pin_3); 


Sensor_GOIO_Iint();

 
/*

u16 value;
 u8 valueh;
 u8 valuel;


*/



//GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
//MyDev1();//����  �������Ѹ�115200
 


MyDev2();//����

}




#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {}
}
#endif