static void PrintArray(uint8_t* buffer)
{
    LcdClearChip(1);
    LcdClearChip(2);
    int iter;
    for(iter = 0; iter < 128; iter++)
        SetPixel(iter, buffer[iter]);
}

static void SetPixel(uint8_t x, uint8_t y)
{
    if(x > 127 || y > 63)
      return;
    y = 63-y;
    //x calc
    uint8_t chipnum;
    if (x < 64) chipnum = 1;
    else chipnum = 2;
    SetAdress(chipnum, x % 64);
    
    //y calc
    SetPage(chipnum, y / 8);
    uint8_t y_off = y % 8;
    uint8_t fin = 0x01;
    fin <<= y_off;
    
    //
    WriteData(chipnum, fin);
}