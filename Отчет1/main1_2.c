/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DELAY_MIN 250000
#define DELAY_MAX 500000
#define step 10000
#define contact_bounce 1000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile int btn_state = 0; 
static int state = 0;
static int past_state = 0;
/* Private function prototypes -----------------------------------------------*/
static void Delay( uint32_t delay );
static void PeriphInit( void );
static int Poll();
static void SwitchState();
static void LightUpLEDs();
/* Private functions ---------------------------------------------------------*/


int main()
{
    PeriphInit();
    while(1)
    { 
        if(Poll()) //если состояние кнопки отжата => нажата
        {
            SwitchState(); //изменение состояние системы
            LightUpLEDs(); //реакция на изменение состояния
        }
    }
}

static void LightUpLEDs()
{
    switch (state) 
    {
        case 0 : 
        {
            //0 0
            PORT_ResetBits( MDR_PORTC, PORT_Pin_0 );
            PORT_ResetBits( MDR_PORTC, PORT_Pin_1 );
            break;
        }
        case 1 : 
        {
            //0 1
            PORT_ResetBits( MDR_PORTC, PORT_Pin_0 );
            PORT_SetBits( MDR_PORTC, PORT_Pin_1 );
            break;
        }
        case 2 : 
        {
            //1 0
            PORT_SetBits( MDR_PORTC, PORT_Pin_0 );
            PORT_ResetBits( MDR_PORTC, PORT_Pin_1 );
            break;
        }
        case 3 : 
        {
            //1 1
            PORT_SetBits( MDR_PORTC, PORT_Pin_0 );
            PORT_SetBits( MDR_PORTC, PORT_Pin_1 );
            break;
        }
    }
}

static void SwitchState()
{
   if (state == 3) state = 0;
   else state++;
}

static int Poll() //опрос кнопки
{
    btn_state = !PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_2);
    if(btn_state != past_state)
    {
        past_state = btn_state;
        if(btn_state)
            return 1;     
    } 
    return 0;
}

static void Delay( uint32_t delay )
{
    while( --delay )
    {
        __NOP();
    }
}

static void PeriphInit( void )
{
    PORT_InitTypeDef PORT_InitStruct;
    /* Включение тактирования порта C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE, ENABLE);
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
    
    PORT_InitStruct.PORT_Pin       = PORT_Pin_2;
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_Init(MDR_PORTC, &PORT_InitStruct);
    PORT_InitStruct.PORT_Pin       = PORT_Pin_1;
    PORT_Init(MDR_PORTE, &PORT_InitStruct);
    PORT_InitStruct.PORT_Pin       = PORT_Pin_5;
    PORT_Init(MDR_PORTB, &PORT_InitStruct);
    
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
