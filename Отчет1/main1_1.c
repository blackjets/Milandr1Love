#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define DELAY 500000

static void Delay( uint32_t delay );

void frq_init(void);
static void PeriphInit( void );
int check_btn(void);

void main(){
  frq_init();
  PeriphInit();
  while(1){
    PORT_SetBits(MDR_PORTC, PORT_Pin_0);
    Delay( DELAY );
    PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
    Delay( DELAY );
  }
}

void frq_init(void)
{
  MDR_RST_CLK->HS_CONTROL = 0x1; // Enable HSE oscillator
  /* wait while HSE startup */
  while (MDR_RST_CLK->CLOCK_STATUS == 0x00) __NOP();
  MDR_RST_CLK->CPU_CLOCK = 0x102; // switch to HSE (8 MHz)
  SystemCoreClockUpdate();
}
static void Delay( uint32_t delay ){
  if(PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5)== Bit_SET) delay*=2;
  if(PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_1)== Bit_SET) delay/=2;
  while( --delay ){
      __NOP();
  }
}
static void PeriphInit( void )
{
    PORT_InitTypeDef PORT_InitStruct;
    
    //Светодиоды
    /* Включение тактирования порта C */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
    /* Настройка порта C для вывода в дискретном режиме. */
    PORT_InitStruct.PORT_OE        = PORT_OE_OUT;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_InitStruct.PORT_Pin       = PORT_Pin_0 | PORT_Pin_1;
    
    PORT_Init(MDR_PORTC, &PORT_InitStruct);
    
    //Кнопка SELECT
    //Тактирование порта уже включено
    /* Настройка порта C для входа в дискретном режиме. */
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_InitStruct.PORT_Pin       = PORT_Pin_2;
    
    PORT_Init(MDR_PORTC, &PORT_InitStruct);
    
    //Кнопки UP RIGHT
    /* Включение тактирования порта B */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
    /* Настройка порта B для входа в дискретном режиме. */
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_InitStruct.PORT_Pin=PORT_Pin_5 | PORT_Pin_6;

    PORT_Init(MDR_PORTB, &PORT_InitStruct);

    //Кнопки DOWN LEFT
    /* Включение тактирования порта E */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
    /* Настройка порта E для входа в дискретном режиме. */
    PORT_InitStruct.PORT_OE        = PORT_OE_IN;
    PORT_InitStruct.PORT_FUNC      = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE      = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED     = PORT_SPEED_SLOW;
    PORT_InitStruct.PORT_PULL_UP   = PORT_PULL_UP_OFF;
    PORT_InitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PORT_InitStruct.PORT_PD_SHM    = PORT_PD_SHM_OFF;
    PORT_InitStruct.PORT_PD        = PORT_PD_DRIVER;
    PORT_InitStruct.PORT_GFEN      = PORT_GFEN_OFF;
    PORT_InitStruct.PORT_Pin       = PORT_Pin_1 | PORT_Pin_3;
    PORT_Init(MDR_PORTE, &PORT_InitStruct);
    
}
