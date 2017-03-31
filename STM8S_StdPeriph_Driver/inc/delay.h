#ifndef __STM8S_DELAY_H
#define __STM8S_DELAY_H

void delay_us(uint16_t x)
{
     while(x--)
     {
           //nop();
           //nop();
           //nop();
           //nop();
           nop();
           nop();
           nop();
           nop();
           nop();
           nop();
           nop();
     }
}

void delay_ms(uint16_t x)
{
     while(x--)
     {
        delay_us(1000);
     }
}
#endif /* __STM8S_DELAY_H */