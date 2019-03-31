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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Delay( uint32_t delay );
static void InitLeds( void );
static void InitTimers( void );
static void InitButtons( void );
/* Private functions ---------------------------------------------------------*/

int main()
{
    //PeriphInit();
    InitLeds();
    InitButtons();
    InitTimers();
    
    while(1)
    {
        if (PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_1))
        {
            NVIC_SetPriority(Timer1_IRQn, 1);
            NVIC_SetPriority(Timer2_IRQn, 0); 
        }
        else 
        {
            NVIC_SetPriority(Timer1_IRQn, 0);
            NVIC_SetPriority(Timer2_IRQn, 1);           
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
  
  // “3
}

static void InitLeds( void )
{
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

void Timer1_IRQHandler(void)
{
  TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
  
  uint32_t rxtx = PORT_ReadInputData(MDR_PORTC);
  uint32_t res = rxtx ^ PORT_Pin_0;
  PORT_Write( MDR_PORTC, res );
}

void Timer2_IRQHandler(void)
{
  PORT_SetBits( MDR_PORTC, PORT_Pin_1 );
  Delay(2000000);
  TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
  PORT_ResetBits( MDR_PORTC, PORT_Pin_1 );
}

#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr)
{
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

/* END OF FILE main.c */
