/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Task 2
#define DELAY_MIN 250000
#define DELAY_MAX 500000
#define step 10000
// Частота импульсов ШИМ, Гц 
#define PWM_PULSE_F 10000  
// Коэффициент заполнения импульсов в процентах 
#define PWM_PULSE_WIDTH_PERCENT 25  
// Частота импульсов, подаваемых на таймер (CLK), Гц 
#define PWM_TIMER_CLK 8000000 
#define PWM_Pin PORT_Pin_6
#define PWM_PORT MDR_PORTF
#define PWM_TIMER MDR_TIMER1
#define PWM_System_Core_Clock 80000000
#define PWM_PULSE_PERIOD(F) ((uint16_t)((uint32_t)(PWM_TIMER_CLK) / (uint32_t)(F)) - 1)

#define PWM_CHN TIMER_CHANNEL1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Delay( uint32_t delay );
static void InitLeds( void );
static void InitTimers( void );
static void InitPWM( void );
static void InitButtons( void );
void U_PWM_Set_Pulse_Width_Percent (uint16_t width_percent);
/* Private functions ---------------------------------------------------------*/

int main()
{
    InitLeds();
    InitButtons();
    if(Task == 1)
      InitTimers();
    else if (Task == 2)
      InitPWM();
    uint32_t TIM_PULSE =  50;
     uint8_t select;
      uint8_t up;
      uint8_t down;
    
    while(1)
    { 
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
      } 
      Delay(step);
    }
    
}
static void Delay( uint32_t delay )
{
    while( --delay )
    {
        __NOP();
    }
}

static void InitLeds( void )
{
    PORT_InitTypeDef PORT_InitStruct;
    /* Включение тактирования порта C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    /* Настройка порта C.0 для вывода в дискретном режиме. */
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
  //Структура настройки таймера
  TIMER_CntInitTypeDef TIMER_CntInitStruct;
  //Структура настройки КАНАЛА таймера
  TIMER_ChnInitTypeDef TimerChnInitStructure;
  //Структура настройки ВЫХОДА канала таймера
  TIMER_ChnOutInitTypeDef TimerChnOutInitStructure;
  
  // Включаем тактирование для Т1 и Т2
  RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1| RST_CLK_PCLK_TIMER2, ENABLE);
  // Заполняет поля структуры значениями по умолчанию      
  TIMER_CntStructInit(&TIMER_CntInitStruct);
  
  // Установка периода для Т1
  TIMER_CntInitStruct.TIMER_Period = 31250;     
  // Инициализация настроек Т1
  TIMER_CntInit(MDR_TIMER1, &TIMER_CntInitStruct);
  // Разрешение прерывания от таймера 1 по переполнению  и установка предделителя 
  TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv128);
  TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);          
  // Установка периода для Т2
  TIMER_CntInitStruct.TIMER_Period = 62500; 
  // Инициализация настроек Т2
  TIMER_CntInit(MDR_TIMER2, &TIMER_CntInitStruct); 
  // Разрешение прерывания от таймера 2 по переполнению     
  TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv128);
  // Разрешение прерывания от таймера 2 по переполнению 
  TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE);  
  
  // Установка приоритетов для Т1 и Т2    
  NVIC_SetPriorityGrouping(3);
  NVIC_SetPriority(Timer1_IRQn, 0);
  NVIC_SetPriority(Timer2_IRQn, 0);
  // Разрешение прерываний для Т1 и Т2 
  NVIC_EnableIRQ(Timer1_IRQn);
  NVIC_EnableIRQ(Timer2_IRQn);
  
  // Включение таймера 1 и 2
  TIMER_Cmd(MDR_TIMER1, ENABLE);
  
    /* НАСТРОЙКА КАНАЛА ТАЙМЕРА 1 */
  //Заполняем структуру стандартными значениями
  TIMER_ChnStructInit (&TimerChnInitStructure);
  TimerChnInitStructure.TIMER_CH_Number = TIMER_CHANNEL1;
  //Режим работы - ШИМ
  TimerChnInitStructure.TIMER_CH_Mode = TIMER_CH_MODE_PWM;
  TimerChnInitStructure.TIMER_CH_REF_Format = TIMER_CH_REF_Format6;
  
  //Настраиваем канал_1 таймер_1 заполенной структурой
  TIMER_ChnInit (MDR_TIMER1, &TimerChnInitStructure);
  
  /* НАСТРОЙКА ВЫХОДА канала ТАЙМЕРА */
  //Заполняем структуру стандартными значениями
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
  
  //Настраиваем выход канал_1 таймер_1 заполенной структурой
  TIMER_ChnOutInit (MDR_TIMER1, &TimerChnOutInitStructure);
  
  TIMER_Cmd(MDR_TIMER2, ENABLE);
  
  // Т3 
  
  
}
static void InitButtons(void)
{
      PORT_InitTypeDef PORT_InitStruct;
    /* Включение тактирования порта C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE, ENABLE);
    /* Настройка порта C.0 для вывода в дискретном режиме. */
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

void Timer1_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
  MDR_PORTC->RXTX ^= PORT_Pin_0;
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
  // Вычислить ширину импульса,  
  // взяв нужное количество % от его периода 
  width = PWM_PULSE_PERIOD (PWM_PULSE_F) * width_percent / 100; 
  TIMER_SetChnCompare (PWM_TIMER, PWM_CHN, width); 
} 

/* END OF FILE main.c */
