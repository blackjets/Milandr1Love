    while(1)
    { 
      select = PORT_ReadInputDataBit(MDR_PORTC,PORT_Pin_2);
      up = PORT_ReadInputDataBit(MDR_PORTB,PORT_Pin_5);
      down = PORT_ReadInputDataBit(MDR_PORTE,PORT_Pin_1);
      
      if (select == 0) 
      {
        NVIC_SetPriority(Timer1_IRQn, 0);
        NVIC_SetPriority(Timer2_IRQn, 0);
      }
      else if (up == 0)
      {
        NVIC_SetPriority(Timer1_IRQn, 1);
        NVIC_SetPriority(Timer2_IRQn, 0);
      }
      else if (down == 0)
      {
        NVIC_SetPriority(Timer1_IRQn, 0);
        NVIC_SetPriority(Timer2_IRQn, 1);
      } 
    }