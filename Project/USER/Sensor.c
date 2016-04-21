#include"Sensor.h"


volatile u8 fac_us=0; //us��ʱ������

//��ʱ������ʼ��
//Ϊȷ��׼ȷ��,�뱣֤ʱ��Ƶ�����Ϊ4�ı���,���8Mhz
//clk:ʱ��Ƶ��(24/16/12/8��)
void delay_init(u8 clk)
{
 if(clk>16)fac_us=(16-4)/4;//24Mhzʱ,stm8���19������Ϊ1us
 else if(clk>4)fac_us=(clk-4)/4;
 else fac_us=1;
}
//��ʱnus
//��ʱʱ��=(fac_us*4+4)*nus*(T)
//����,TΪCPU����Ƶ��(Mhz)�ĵ���,��λΪus.
//׼ȷ��:
//92%  @24Mhz
//98%  @16Mhz
//98%  @12Mhz
//86%  @8Mhz
void delay_us(u16 nus)
{
__asm(
"PUSH A          \n"  //1T,ѹջ
"DELAY_XUS:      \n"
"LD A,fac_us     \n"   //1T,fac_us���ص��ۼ���A
"DELAY_US_1:     \n"
"NOP             \n"  //1T,nop��ʱ
"DEC A           \n"  //1T,A--
"JRNE DELAY_US_1 \n"   //������0,����ת(2T)��DELAY_US_1����ִ��,������0,����ת(1T).
"NOP             \n"  //1T,nop��ʱ
"DECW X          \n"  //1T,x--
"JRNE DELAY_XUS  \n"    //������0,����ת(2T)��DELAY_XUS����ִ��,������0,����ת(1T).
"POP A           \n"  //1T,��ջ
);
}
//��ʱnms
//Ϊ��֤׼ȷ��,nms��Ҫ����16640.
void delay_ms(u32 nms)
{
 u8 t;
 if(nms>65)
 {
  t=nms/65;
  while(t--)delay_us(65000);
  nms=nms%65;
 }
 delay_us(nms*1000);
}

void Sensor_GOIO_Iint(void)
{
  GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);//T1
  GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);//T2
  
   EXTI_DeInit();
 // EXTI_SelectPort(EXTI_Port_D);
  
  //GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);//R2
  GPIO_Init(GPIOD, GPIO_Pin_5,GPIO_Mode_In_FL_No_IT);//R1��������
  //EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising);

  
  GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_Out_OD_HiZ_Fast);//����˿�ʹ��
 }
//������·״̬�෴�Ĳ�
u8 fuck;
void Sensor_Start(void)
{ 
  for(fuck=0;fuck<8;fuck++)
  {
  GPIOD->ODR |= GPIO_Pin_1;//�ø�
  GPIOD->ODR &= (uint8_t)(~GPIO_Pin_2);
  delay_us(11);
  GPIOD->ODR &= (uint8_t)(~GPIO_Pin_1);//����
  GPIOD->ODR |= GPIO_Pin_2;
  delay_us(11);
  }
 
}

//׼�����յķ����źţ�ռ�ձȿɵ���0<high<22
void Sensor_FlagTrig(uint8_t high)
{
  GPIOD->ODR |= GPIO_Pin_4;//�ø�  
  delay_us(high);
  GPIOD->ODR &= (uint8_t)(~GPIO_Pin_4);//����
  delay_us(20-high);
}








/*
u8 DelayUs(void)//��ʱ10΢��
{
  u8 temp;
  for(temp=0;temp<16;temp++);
  return temp;
}
*/
 