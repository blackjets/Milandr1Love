/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_adc.h"
#include "lcd/my_lcd.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Task 3
#define DELAY_MIN 250000
#define DELAY_MAX 500000
#define step 10000
// ×àñòîòà èìïóëüñîâ ØÈÌ, Ãö 
#define PWM_PULSE_F 10000  
// Êîýôôèöèåíò çàïîëíåíèÿ èìïóëüñîâ â ïðîöåíòàõ 
#define PWM_PULSE_WIDTH_PERCENT 25  
// ×àñòîòà èìïóëüñîâ, ïîäàâàåìûõ íà òàéìåð (CLK), Ãö 
#define PWM_TIMER_CLK 8000000 
#define PWM_Pin PORT_Pin_6
#define PWM_PORT MDR_PORTF
#define PWM_TIMER MDR_TIMER1
#define PWM_System_Core_Clock 80000000
#define PWM_PULSE_PERIOD(F) ((uint16_t)((uint32_t)(PWM_TIMER_CLK) / (uint32_t)(F)) - 1)
#define DELIMITER 2

#define PWM_CHN TIMER_CHANNEL1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Delay( uint32_t delay );
static void InitLeds( void );
static void InitTimers( void );
static void InitPWM( void );
static void InitButtons( void );
static void InitADC(void);
static void SetPixel(uint8_t x, uint8_t y);
static void normalizeBuffer(uint8_t *buff);
//static void SetPoint(uint8_t x, uint8_t y);
static void PrintArray(uint8_t* buffer);
void U_PWM_Set_Pulse_Width_Percent (uint16_t width_percent);
void frq_init();

uint8_t temp = 1;
  uint32_t status = 1;
  int iBuffer = 0;
  FlagStatus flag;
  int flagShtock;
  uint8_t interruptBuffer[128];
  uint8_t tempBuffer[128];
  float scale = 1;
  int j = 0;
  int tmp_min = 255;
  int tmp_max = 0;
  int tmp = 0;
  bool scalingFlag = false;
  
/* Private functions ---------------------------------------------------------*/

int main()
{
  frq_init();
    LcdPeriphInit();
    LcdInit();
    DispOn(1);
    DispOn(2);
    LcdClearChip(1);
    LcdClearChip(2);
    
    InitLeds();
    InitButtons();
    if(Task == 1)
      InitTimers();
    else if (Task == 2)
      InitPWM();
    else if (Task == 3)
      InitTimers();
      InitADC();
    uint32_t TIM_PULSE =  50;
     uint8_t select;
      uint8_t up;
      uint8_t down;
    
    while(1)
    { 
      
      /*
      select = PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_2);
      up = PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5);
      down = PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_1);
      
      if (select == 0) 
      {
        TIM_PULSE = 3;
        PWM_TIMER->CCR1 = TIM_PULSE; 
      }
      else if (up == 0 && TIM_PULSE < 800)
      {
        TIM_PULSE++;
        PWM_TIMER->CCR1 = TIM_PULSE; 
      }
      else if (down == 0 && TIM_PULSE >0)
      {
        TIM_PULSE--;
        PWM_TIMER->CCR1 = TIM_PULSE; 
      } */
      
      //PrintArray(tempBuffer);
      //Delay(step);
    }
    
}
static void Delay( uint32_t delay )
{
    while( --delay )
    {
        __NOP();
    }
}
void frq_init(void){
//  MDR_RST_CLK->HS_CONTROL = 0x1; // Enable HSE oscillator
//  /* wait while HSE startup */
//  while (MDR_RST_CLK->CLOCK_STATUS == 0x00) __NOP();
//  MDR_RST_CLK->CPU_CLOCK = 0x102; // switch to HSE (8 MHz)
//  SystemCoreClockUpdate();
  
  //Ñîñòîÿíèå ïîñëå ñáðîñà
  // HSE - Off , LSE - OFF
  // SystemCoreClock = 8000000 HSI
 
  //Çàïóñêàåì òàêòèðîâàíèå îò âíåøíåãî ðåçîíàòîðà
  RST_CLK_HSEconfig(RST_CLK_HSE_ON);
  //Æä¸ì ïîêà îí íå âûéäåò íà ðàáî÷óþ ÷àñòîòó
  while (ERROR == RST_CLK_HSEstatus()) __NOP();
  //HSE âêëþ÷¸í, íî åù¸ íå èñïîëüçóåòñÿ
  
  //Íàñòðàèâàåì ìíîæèòåëü ÷àñòîòû
  //Èñòî÷íèê HSE, ïðåääåëèòåëü - 1
  //Èñïîëüçóåìûé ìíîæèòåëü ÷àñòîòû - 10
  RST_CLK_CPU_PLLconfig(RST_CLK_USB_PLLsrcHSEdiv1, RST_CLK_USB_PLLmul10);
  //Çàïóñêàåì ìíîæèòåëü ÷àñòîòû
  RST_CLK_CPU_PLLcmd(ENABLE);
  //Æä¸ì ïîêà îí íå âûéäåò íà ðàáî÷óþ ÷àñòîòó
  while (ERROR == RST_CLK_CPU_PLLstatus()) __NOP();
  
  //Óñòàíàâëèâàåì ïðåääåëèòåëü ñèñòåìíîé ÷àñòîòû
  RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
  //Èñòî÷íèêîì ñèñòåìíîé ÷àñòîòû âûáèðàåì CPU_C3 - HSE->/1->Pll(*10)->/1
  RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
  //Ïåðåêëþ÷àåì òàêñòèðîâàíèå ñèñòåìíîé øèíû CPU_C2_SEL
  RST_CLK_CPU_PLLuse(ENABLE);
  
  //Îáíîâëÿåì ñïðàâî÷íîå çíà÷åíèå ñèñòåìíîé ÷àñòîòû
  SystemCoreClockUpdate();
}
static void InitLeds( void )
{
    PORT_InitTypeDef PORT_InitStruct;
    /* Âêëþ÷åíèå òàêòèðîâàíèÿ ïîðòà C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    /* Íàñòðîéêà ïîðòà C.0 äëÿ âûâîäà â äèñêðåòíîì ðåæèìå. */
    PORT_InitStruct.PORT_Pin       = PORT_Pin_1 | PORT_Pin_0;
    PORT_InitStruct.PORT_OE        = PORT_OE_OUT;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_Init(MDR_PORTC, &PORT_InitStruct); 
}
static void InitTimers(void)
{
  //Ñòðóêòóðà íàñòðîéêè òàéìåðà
  TIMER_CntInitTypeDef TIMER_CntInitStruct;
  //Ñòðóêòóðà íàñòðîéêè ÊÀÍÀËÀ òàéìåðà
  TIMER_ChnInitTypeDef TimerChnInitStructure;
  //Ñòðóêòóðà íàñòðîéêè ÂÛÕÎÄÀ êàíàëà òàéìåðà
  TIMER_ChnOutInitTypeDef TimerChnOutInitStructure;
  
  // Âêëþ÷àåì òàêòèðîâàíèå äëÿ Ò1 è Ò2
  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1| RST_CLK_PCLK_TIMER2, ENABLE);
  // Çàïîëíÿåò ïîëÿ ñòðóêòóðû çíà÷åíèÿìè ïî óìîë÷àíèþ      
  TIMER_CntStructInit(&TIMER_CntInitStruct);
  
  // Óñòàíîâêà ïåðèîäà äëÿ Ò1
  TIMER_CntInitStruct.TIMER_Period = 50;     
  // Èíèöèàëèçàöèÿ íàñòðîåê Ò1
  TIMER_CntInit(MDR_TIMER1, &TIMER_CntInitStruct);
  // Ðàçðåøåíèå ïðåðûâàíèÿ îò òàéìåðà 1 ïî ïåðåïîëíåíèþ  è óñòàíîâêà ïðåääåëèòåëÿ 
  TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv128);
  TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);          
  // Óñòàíîâêà ïåðèîäà äëÿ Ò2
  TIMER_CntInitStruct.TIMER_Period = 62500; 
  // Èíèöèàëèçàöèÿ íàñòðîåê Ò2
  TIMER_CntInit(MDR_TIMER2, &TIMER_CntInitStruct); 
  // Ðàçðåøåíèå ïðåðûâàíèÿ îò òàéìåðà 2 ïî ïåðåïîëíåíèþ     
  TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv128);
  // Ðàçðåøåíèå ïðåðûâàíèÿ îò òàéìåðà 2 ïî ïåðåïîëíåíèþ 
  TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE);  
  
  // Óñòàíîâêà ïðèîðèòåòîâ äëÿ Ò1 è Ò2    
  NVIC_SetPriorityGrouping(3);
  NVIC_SetPriority(Timer1_IRQn, 0);
  NVIC_SetPriority(Timer2_IRQn, 0);
  // Ðàçðåøåíèå ïðåðûâàíèé äëÿ Ò1 è Ò2 
  NVIC_EnableIRQ(Timer1_IRQn);
  NVIC_EnableIRQ(Timer2_IRQn);
  
  // Âêëþ÷åíèå òàéìåðà 1 è 2
  TIMER_Cmd(MDR_TIMER1, ENABLE);
  
    /* ÍÀÑÒÐÎÉÊÀ ÊÀÍÀËÀ ÒÀÉÌÅÐÀ 1 */
  //Çàïîëíÿåì ñòðóêòóðó ñòàíäàðòíûìè çíà÷åíèÿìè
  TIMER_ChnStructInit (&TimerChnInitStructure);
  TimerChnInitStructure.TIMER_CH_Number = TIMER_CHANNEL1;
  //Ðåæèì ðàáîòû - ØÈÌ
  TimerChnInitStructure.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
  TimerChnInitStructure.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
  
  //Íàñòðàèâàåì êàíàë_1 òàéìåð_1 çàïîëåííîé ñòðóêòóðîé
  TIMER_ChnInit (MDR_TIMER1, &TimerChnInitStructure);
  
  /* ÍÀÑÒÐÎÉÊÀ ÂÛÕÎÄÀ êàíàëà ÒÀÉÌÅÐÀ */
  //Çàïîëíÿåì ñòðóêòóðó ñòàíäàðòíûìè çíà÷åíèÿìè
  TIMER_ChnOutStructInit (&TimerChnOutInitStructure);
  
  TimerChnOutInitStructure.TIMER_CH_Number = TIMER_CHANNEL1;
  TimerChnOutInitStructure.TIMER_CH_DirOut_Polarity =
                                              TIMER_CHOPolarity_NonInverted;
  TimerChnOutInitStructure.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF;
  TimerChnOutInitStructure.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output;
  TimerChnOutInitStructure.TIMER_CH_NegOut_Polarity =
                                              TIMER_CHOPolarity_NonInverted;
  TimerChnOutInitStructure.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF;
  TimerChnOutInitStructure.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output;
  
  //Íàñòðàèâàåì âûõîä êàíàë_1 òàéìåð_1 çàïîëåííîé ñòðóêòóðîé
  TIMER_ChnOutInit (MDR_TIMER1, &TimerChnOutInitStructure);
  
  TIMER_Cmd(MDR_TIMER2, ENABLE);
  
  // Ò3 
  
  
}
static void InitButtons(void)
{
      PORT_InitTypeDef PORT_InitStruct;
    /* Âêëþ÷åíèå òàêòèðîâàíèÿ ïîðòà C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE, ENABLE);
    /* Íàñòðîéêà ïîðòà C.0 äëÿ âûâîäà â äèñêðåòíîì ðåæèìå. */
    PORT_InitStruct.PORT_Pin       = PORT_Pin_2;
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    
    PORT_Init(MDR_PORTC, &PORT_InitStruct);
    PORT_InitStruct.PORT_Pin       = PORT_Pin_1;
    PORT_Init(MDR_PORTE, &PORT_InitStruct);
    PORT_InitStruct.PORT_Pin       = PORT_Pin_5;
    PORT_Init(MDR_PORTB, &PORT_InitStruct);
}
static void InitPWM(void)
{
  uint16_t PrescalerValue; 
  TIMER_CntInitTypeDef  TimerInitStructure;
  TIMER_ChnInitTypeDef TimerChnInitStructure; 
  TIMER_ChnOutInitTypeDef TimerChnOutInitStructure;
  PORT_InitTypeDef PORT_InitStruct;
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTF | RST_CLK_PCLK_TIMER1, ENABLE); 
  
  
  PORT_StructInit(&PORT_InitStruct); 
  PORT_InitStruct.PORT_FUNC = PORT_FUNC_ALTER; 
  PORT_InitStruct.PORT_OE = PORT_OE_OUT; 
  PORT_InitStruct.PORT_MODE = PORT_MODE_DIGITAL; 
  PORT_InitStruct.PORT_Pin = PWM_Pin; 
  PORT_InitStruct.PORT_SPEED = PORT_SPEED_MAXFAST; 
  PORT_Init (PWM_PORT, &PORT_InitStruct); 
  
  TIMER_DeInit (PWM_TIMER); 
  TIMER_BRGInit (PWM_TIMER, TIMER_HCLKdiv1); 
  PrescalerValue = (uint16_t) (PWM_System_Core_Clock / PWM_TIMER_CLK) - 1; 
  TimerInitStructure.TIMER_Prescaler = PrescalerValue; 
  TimerInitStructure.TIMER_Period = PWM_PULSE_PERIOD (PWM_PULSE_F); 
  TimerInitStructure.TIMER_CounterDirection = TIMER_CntDir_Up; 
  TimerInitStructure.TIMER_CounterMode = TIMER_CntMode_ClkFixedDir; 
  TimerInitStructure.TIMER_EventSource = TIMER_EvSrc_None; 
  TimerInitStructure.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1; 
  TimerInitStructure.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately; 
  TimerInitStructure.TIMER_ETR_FilterConf = TIMER_Filter_1FF_at_TIMER_CLK; 
  TimerInitStructure.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None; 
  TimerInitStructure.TIMER_ETR_Polarity = TIMER_ETRPolarity_NonInverted; 
  TimerInitStructure.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted; 
  TIMER_CntInit (PWM_TIMER,&TimerInitStructure); 
  
  TIMER_ChnStructInit (&TimerChnInitStructure); 
  TimerChnInitStructure.TIMER_CH_Number = PWM_CHN; 
  TimerChnInitStructure.TIMER_CH_Mode = TIMER_CH_MODE_PWM; 
  TimerChnInitStructure.TIMER_CH_REF_Format = TIMER_CH_REF_Format6; 
  TIMER_ChnInit (PWM_TIMER, &TimerChnInitStructure); 
  
  TIMER_ChnOutStructInit (&TimerChnOutInitStructure); 
  TimerChnOutInitStructure.TIMER_CH_Number = PWM_CHN; 
  TimerChnOutInitStructure.TIMER_CH_DirOut_Polarity = TIMER_CHOPolarity_NonInverted; 
  TimerChnOutInitStructure.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_REF; 
  TimerChnOutInitStructure.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Output; 
  TimerChnOutInitStructure.TIMER_CH_NegOut_Polarity = TIMER_CHOPolarity_NonInverted; 
  TimerChnOutInitStructure.TIMER_CH_NegOut_Source = TIMER_CH_OutSrc_REF; 
  TimerChnOutInitStructure.TIMER_CH_NegOut_Mode = TIMER_CH_OutMode_Output; 
  TIMER_ChnOutInit (PWM_TIMER, &TimerChnOutInitStructure); 
  
  TIMER_Cmd (PWM_TIMER, ENABLE);
  U_PWM_Set_Pulse_Width_Percent (PWM_PULSE_WIDTH_PERCENT); 

  
}
static void InitADC(void){
  /*ADC_InitTypeDef ADC_InitStruct;
  ADC_StructInit(&ADC_InitStruct);
  ADC_Init(&ADC_InitStruct);*/
  
  /*ADCx_InitTypeDef ADCx_InitStruct;
  ADCx_StructInit(&ADCx_InitStruct);
  ADCx_InitStruct.ADC_ClockSource = ADC_CLOCK_SOURCE_ADC;
  ADCx_InitStruct
  ADC1_Init(&ADCx_InitStruct);
  //ADC_SetTrim(0);
  RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC,ENABLE);
  ADC1_SetHighLevel(1);
  ADC1_SetLowLevel(0);
  ADC1_SetChannel(0);
  ADC1_Start();
  FunctionalState state = ENABLE;
  //ADC1_SamplingModeConfig(1);
  ADC1_LevelsConfig(-1,1,1);*/
  
  
  PORT_InitTypeDef PORT_InitStruct;
  ADC_InitTypeDef sADC;
  ADCx_InitTypeDef sADCx;
    
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
   //PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    //PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_ANALOG;
    //PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    //PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    //PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    //PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    //PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    //PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_InitStruct.PORT_Pin       = PORT_Pin_7;
    PORT_Init(MDR_PORTD, &PORT_InitStruct);
    
  RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC,ENABLE);
    /* Reset all ADC settings */
    ADC_DeInit();
    ADC_StructInit(&sADC);
    
    sADC.ADC_SynchronousMode      = ADC_SyncMode_Independent;           
    sADC.ADC_StartDelay           = 0;
    sADC.ADC_IntVRefConversion    = ADC_VREF_CONVERSION_Disable;
    sADC.ADC_IntVRefTrimming      = 1;
    ADC_Init (&sADC);
    
    /* ADC1 Configuration */
    ADCx_StructInit (&sADCx);
    sADCx.ADC_ClockSource      = ADC_CLOCK_SOURCE_CPU;
    sADCx.ADC_SamplingMode     = ADC_SAMPLING_MODE_SINGLE_CONV;
    sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Enable;
    //sADCx.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable;
    sADCx.ADC_Channels         = ADC_CH_ADC7_MSK;
    sADCx.ADC_LevelControl     = ADC_LEVEL_CONTROL_Disable;
    sADCx.ADC_LowLevel         = 0;
    sADCx.ADC_HighLevel        = 0;
    sADCx.ADC_VRefSource       = ADC_VREF_SOURCE_INTERNAL;
    sADCx.ADC_IntVRefSource    = ADC_INT_VREF_SOURCE_INEXACT;
    sADCx.ADC_Prescaler        = ADC_CLK_div_4;
    sADCx.ADC_DelayGo          = 6;
    ADC1_Init (&sADCx);

    /* Enable ADC1 EOCIF and AWOIFEN interupts */
    //ADC1_ITConfig((ADCx_IT_END_OF_CONVERSION  | ADCx_IT_OUT_OF_RANGE), DISABLE);

    /* ADC1 enable */
    ADC1_Cmd (ENABLE);
  
     
    //ADC1_Start();
    
  
    
  
  
}

void Timer1_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
  //MDR_PORTC->RXTX ^= PORT_Pin_0;
  
  ADC1_Start();
    flag = ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION);
    if(flag == RESET) flagShtock = 0; 
      else flagShtock = 1;
    if(flagShtock)
    {
      status = ADC1_GetStatus();
      //temp = ADC1_GetResult()& 0x0000FFFF;
      temp = ADC1_GetResult() / 16; //uint32 > uint8
     
        interruptBuffer[iBuffer++] = temp ;///(DELIMITER ); 
      if(iBuffer == 128)
      {
          for(j =0; j< 128; j++){
          //interruptBuffer[j] /= 2;
          if(interruptBuffer[j]>tmp_max) tmp_max = interruptBuffer[j];
          if(interruptBuffer[j] < tmp_min) tmp_min = interruptBuffer[j];
          }
        /*flag = false;
        }*/
          tmp = (tmp_max + tmp_min)/2;
          for(j = 0; j<128; j++)
            /*if(interruptBuffer[j] < tmp) interruptBuffer[j]= tmp - interruptBuffer[j];
              else interruptBuffer[j] += temp;*/
            interruptBuffer[j] = 32;
            
          for(j = 0; j<128; j++){
          tempBuffer[j] = interruptBuffer[j];
          }
        
      iBuffer = 0;
      tmp_max  = 0;
      tmp_min = 255;
      }
  }
}

void Timer2_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
  MDR_PORTC->RXTX ^= PORT_Pin_1;

}

#if ( USE_ASSERT_INFO == 1 )
void assert_failed(uint32_t file_id, uint32_t line)
{
  while (1)
  {
  }
}
#elif ( USE_ASSERT_INFO == 2 )
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr)
{
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

void U_PWM_Set_Pulse_Width_Percent (uint16_t width_percent) 
{ 
  uint16_t width;  
  // Âû÷èñëèòü øèðèíó èìïóëüñà,  
  // âçÿâ íóæíîå êîëè÷åñòâî % îò åãî ïåðèîäà 
  width = PWM_PULSE_PERIOD (PWM_PULSE_F) * width_percent / 100; 
  TIMER_SetChnCompare (PWM_TIMER, PWM_CHN, width); 
} 

static void PrintArray(uint8_t* buffer)
{
    LcdClearChip(1);
    LcdClearChip(2);
    int iter;
    for(iter = 0; iter < 128; iter++)
        SetPixel(iter, buffer[iter]);
}

static void SetPixel(uint8_t x, uint8_t y)
{
    if(x > 127 || y > 63)
      return;
    y = 63-y;
    //x calc
    uint8_t chipnum;
    if (x < 64) chipnum = 1;
    else chipnum = 2;
    SetAdress(chipnum, x % 64);
    
    //y calc
    SetPage(chipnum, y / 8);
    uint8_t y_off = y % 8;
    uint8_t fin = 0x01;
    fin <<= y_off;
    
    //
    WriteData(chipnum, fin);
}

static void normalizeBuffer(uint8_t* buff){
  int i = 0;
  uint8_t temp = 0;
  for( i = 0; i<32; i++){
    temp = buff[i] ;
    buff [i] = buff[i+32];
    buff[i+32] = temp;
  }
  
  for( i = 64; i<96; i++){
    temp = buff[i] ;
    buff [i] = buff[i+32];
    buff[i+32] = temp;
  }
}

/* END OF FILE main.c */
