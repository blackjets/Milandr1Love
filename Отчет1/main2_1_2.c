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