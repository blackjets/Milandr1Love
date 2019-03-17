/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TASK 3
//#define DELAY 250000
#define DELAY 1000000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int EXT_INT2 = 0;
/* Private function prototypes -----------------------------------------------*/
static void Delay( uint32_t delay );
static void InitLeds( void );
static void InitTimersForTask1( void );
static void InitButtons( void );
static void InitAdc(void);
/* Private functions ---------------------------------------------------------*/

int main()
{
    InitLeds();
    if(TASK == 1)
      InitTimersForTask1();
    if(TASK == 2){
      InitButtons();
    }
    if (TASK == 3)
      InitAdc();

    while(1)
    { 
      if(EXT_INT2==1 && TASK ==2)
      {
        Delay(DELAY);
        PORT_SetBits( MDR_PORTC, PORT_Pin_1 ); 
        Delay(DELAY);
        PORT_ResetBits( MDR_PORTC, PORT_Pin_1 ); 
        PORT_SetBits( MDR_PORTC, PORT_Pin_0 ); 
        Delay(DELAY);
        PORT_ResetBits( MDR_PORTC, PORT_Pin_0 ); 
        EXT_INT2=0;
      }
      if(TASK==3)
      {
        uint8_t delay = PORT_ReadInputDataBit(MDR_PORTD,PORT_Pin_7);
        PORT_SetBits( MDR_PORTC, PORT_Pin_1 ); 
        Delay(delay*1000000);
        PORT_ResetBits( MDR_PORTC, PORT_Pin_1 ); 
      }
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

static void InitTimersForTask1(void)
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
  TIMER_CntInitStruct.TIMER_Period = 62500;     
  // Инициализация настроек Т1
  TIMER_CntInit(MDR_TIMER1, &TIMER_CntInitStruct);
  // Разрешение прерывания от таймера 1 по переполнению  и установка предделителя 
  TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv128);
  TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);          
  // Установка периода для Т2
  TIMER_CntInitStruct.TIMER_Period = 20000; 
  // Инициализация настроек Т2
  TIMER_CntInit(MDR_TIMER2, &TIMER_CntInitStruct); 
  // Разрешение прерывания от таймера 2 по переполнению     
  TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv128);
  // Разрешение прерывания от таймера 2 по переполнению 
  TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE);  
  
  // Установка приоритетов для Т1 и Т2    
  NVIC_SetPriorityGrouping(3);
  NVIC_SetPriority(Timer1_IRQn, 3);
  NVIC_SetPriority(Timer2_IRQn, 7);
  // Разрешение прерываний для Т1 и Т2 
  NVIC_EnableIRQ(Timer1_IRQn);
  NVIC_EnableIRQ(Timer2_IRQn);
  
  // Включение таймера 1 и 2
  TIMER_Cmd(MDR_TIMER1, ENABLE);
  TIMER_Cmd(MDR_TIMER2, ENABLE);
}

static void InitButtons(void)
{
      PORT_InitTypeDef PORT_InitStruct;
    /* Включение тактирования порта C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    /* Настройка порта B.10 для вывода в дискретном режиме. */
    PORT_InitStruct.PORT_Pin       = PORT_Pin_10;
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_ALTER;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_MAXFAST;//PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_ON;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_ON;
    PORT_Init(MDR_PORTB, &PORT_InitStruct);
    
    NVIC_EnableIRQ(EXT_INT2_IRQn);//Разрешение на внешнее прерывание
    NVIC_SetPriority(EXT_INT2_IRQn, 3);
}

static void InitAdc(void)
{
  ADC_InitTypeDef ADC_InitStructure; 
  ADCx_InitTypeDef ADCx_InitStructure;
  RST_CLK_PCLKcmd (RST_CLK_PCLK_ADC | RST_CLK_PCLK_PORTD, ENABLE); 
  PORT_InitTypeDef PORT_InitStruct;
  PORT_StructInit (&PORT_InitStruct); 
  PORT_InitStruct.PORT_Pin = PORT_Pin_7; 
  PORT_InitStruct.PORT_MODE = PORT_MODE_ANALOG; 
  PORT_Init (MDR_PORTD, &PORT_InitStruct);
  
  ADC_InitStructure.ADC_SynchronousMode = ADC_SyncMode_Independent; 
  ADC_InitStructure.ADC_StartDelay = 0; 
  ADC_InitStructure.ADC_TempSensor = ADC_TEMP_SENSOR_Disable; 
  ADC_InitStructure.ADC_TempSensorAmplifier = ADC_TEMP_SENSOR_AMPLIFIER_Disable; 
  ADC_InitStructure.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Disable; 
  ADC_InitStructure.ADC_IntVRefConversion = ADC_VREF_CONVERSION_Disable; 
  ADC_InitStructure.ADC_IntVRefTrimming = 0; 
  ADC_Init (&ADC_InitStructure); 
  
  ADCx_StructInit (&ADCx_InitStructure); 
  ADCx_InitStructure.ADC_ClockSource = ADC_CLOCK_SOURCE_CPU; 
  ADCx_InitStructure.ADC_Prescaler = ADC_CLK_div_512; 
  ADCx_InitStructure.ADC_SamplingMode = ADC_SAMPLING_MODE_SINGLE_CONV; 
  ADCx_InitStructure.ADC_ChannelSwitching = ADC_CH_SWITCHING_Disable; 
  ADCx_InitStructure.ADC_ChannelNumber = PORT_Pin_7; 
  ADCx_InitStructure.ADC_Channels = 0; 
  ADCx_InitStructure.ADC_DelayGo = 7; 
  ADCx_InitStructure.ADC_LevelControl = ADC_LEVEL_CONTROL_Disable; 
  ADCx_InitStructure.ADC_LowLevel = 0; 
  ADCx_InitStructure.ADC_HighLevel = 0; 
  ADCx_InitStructure.ADC_VRefSource = ADC_VREF_SOURCE_INTERNAL; 
  ADCx_InitStructure.ADC_IntVRefSource = ADC_INT_VREF_SOURCE_INEXACT; 
  ADC1_Init (&ADCx_InitStructure); 
}
void EXT_INT2_IRQHandler(void){
   EXT_INT2 = 1;
}

void Timer1_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
  PORT_SetBits( MDR_PORTC, PORT_Pin_0 );
  Delay(DELAY);
  PORT_ResetBits( MDR_PORTC, PORT_Pin_0 ); 
}

void Timer2_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
  PORT_SetBits( MDR_PORTC, PORT_Pin_1 );
  //PORT_ResetBits( MDR_PORTC, PORT_Pin_0 ); 
  Delay(DELAY);
  //PORT_SetBits( MDR_PORTC, PORT_Pin_0 );
  PORT_ResetBits( MDR_PORTC, PORT_Pin_1 ); 
}

__task void U_ADC_Task_Function (void) { 
  uint32_t DU;  // Результат аналого-цифрового преобразования 
  float U;      // Измеренное напряжение  
  while(1) { 
    // Запустить процесс аналого-цифрового преобразования 
    ADC1_Start();  
    // Дождаться завершения преобразования, постоянно опрашивая 
    // соответствующий флаг в регистре состояния АЦП1 
    while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0); 
 
    // os_dly_wait (1); 
    // Получить результат преобразования 
    DU = ADC1_GetResult() & 0x0000FFFF;  
    // Преобразование показаний АЦП в измеренное напряжение 
    U = (U_ADC_U / U_ADC_D) * DU; 
    os_dly_wait (250); 

  } 
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

/* END OF FILE main.c */
