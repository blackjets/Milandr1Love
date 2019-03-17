/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_adc.h"
#include "lvgl/lvgl.h"

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

/* Private functions ---------------------------------------------------------*/

int main()
{
    lv_init();

    while(1)
    { 
      
    }
    
}
static void Delay( uint32_t delay )
{
    while( --delay )
    {
        __NOP();
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
