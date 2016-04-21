#include "Myhalt.h"
#include "stm8l15x.h"
void AWU_Config(void)
{
  //AWU_DeInit();
  //AWU_LSICalibrationConfig(128000);
  // AWU_Init(AWU_TIMEBASE_1S);
  
}
void RTC_Config(u16 time) 
{  
   RTC_DeInit(); //��ʼ��Ĭ��״̬ 
   CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); //����RTCʱ�� 
   CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_2); //ѡ��RTCʱ��ԴLSI��2=19K 
   RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16); //19k/16=1.1875KHz t=0.85ms 
   RTC_ITConfig(RTC_IT_WUT, ENABLE); //�����ж� 
  RTC_SetWakeUpCounter(time); //����RTC Weakup��������ֵ 
  RTC_WakeUpCmd(ENABLE); //ʹ���Զ����� 
} 


