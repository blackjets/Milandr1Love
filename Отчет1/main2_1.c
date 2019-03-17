/* Includes ------------------------------------------------------------------*/

#include "MDR32Fx.h" 
#include "MDR32F9Qx_config.h" 
#include "MDR32F9Qx_port.h" 
#include "MDR32F9Qx_rst_clk.h" 
#include "MDR32F9Qx_timer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void PeriphInit(void);
/* Private functions ---------------------------------------------------------*/

int main()
{
    PeriphInit();

    while(1)
    {
    }
}

static void PeriphInit( void )
{
    PORT_InitTypeDef PORT_InitStruct;
    TIMER_CntInitTypeDef TIMER_CntInitStruct;
    
    // ----------------------------------------------------
    //  ENABLE RCC
    // ----------------------------------------------------
    
    RST_CLK_PCLKcmd( RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_PORTC
                    | RST_CLK_PCLK_TIMER1 | RST_CLK_PCLK_TIMER2,
                    ENABLE );
    
    // ----------------------------------------------------
    //  INIT PORTC
    // ----------------------------------------------------
    
    PORT_StructInit(&PORT_InitStruct);
    
    PORT_InitStruct.PORT_Pin   = ( PORT_Pin_0 | PORT_Pin_1 );
    PORT_InitStruct.PORT_OE    = PORT_OE_OUT;
    PORT_InitStruct.PORT_FUNC  = PORT_FUNC_PORT;
    PORT_InitStruct.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStruct.PORT_SPEED = PORT_SPEED_SLOW;
    PORT_Init( MDR_PORTC, &PORT_InitStruct );
    
    PORT_InitStruct.PORT_Pin   = PORT_Pin_2;
    PORT_InitStruct.PORT_OE    = PORT_OE_IN;
    PORT_Init( MDR_PORTC, &PORT_InitStruct );
    
    // ----------------------------------------------------
    //  INIT TIMER 1, 2
    // ----------------------------------------------------
    
    TIMER_CntStructInit(&TIMER_CntInitStruct);
    
    TIMER_CntInitStruct.TIMER_Period = 100;
    TIMER_CntInit( MDR_TIMER1, &TIMER_CntInitStruct );
    
    TIMER_BRGInit( MDR_TIMER1, TIMER_HCLKdiv1);
    TIMER_ITConfig( MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE );
    
    TIMER_CntInitStruct.TIMER_Period = 40000;
    TIMER_CntInit( MDR_TIMER2, &TIMER_CntInitStruct ); 
    
    TIMER_BRGInit( MDR_TIMER2, TIMER_HCLKdiv1 );
    TIMER_ITConfig( MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE );
    
    // ----------------------------------------------------
    //  INIT NVIC
    // ----------------------------------------------------
    NVIC_SetPriorityGrouping( 3 ); 
    NVIC_SetPriority( Timer1_IRQn, 0 ); 
    NVIC_SetPriority( Timer2_IRQn, 0 ); 
    NVIC_EnableIRQ( Timer1_IRQn ); 
    NVIC_EnableIRQ( Timer2_IRQn ); 

    TIMER_Cmd( MDR_TIMER1, ENABLE ); 
    TIMER_Cmd( MDR_TIMER2, ENABLE );
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
