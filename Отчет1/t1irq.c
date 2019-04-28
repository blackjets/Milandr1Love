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