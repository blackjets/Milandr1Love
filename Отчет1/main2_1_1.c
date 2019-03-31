// Установка приоритетов для Т1 и Т2    
  NVIC_SetPriorityGrouping(3);
  NVIC_SetPriority(Timer1_IRQn, 0);
  NVIC_SetPriority(Timer2_IRQn, 0);
  // Разрешение прерываний для Т1 и Т2 
  NVIC_EnableIRQ(Timer1_IRQn);
  NVIC_EnableIRQ(Timer2_IRQn);
  
  // Включение таймера 1 и 2
  TIMER_Cmd(MDR_TIMER1, ENABLE);

  // Установка периода для Т1
  TIMER_CntInitStruct.TIMER_Period = 31250;   
  // Установка периода для Т2
  TIMER_CntInitStruct.TIMER_Period = 62500; 