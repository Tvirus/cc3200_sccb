#include "cc3200_delay.h"
#include "rom_map.h"


void CC3200_Delay_ms(unsigned int ms)
{
    while (ms--)
        MAP_UtilsDelay(13333);
}


void CC3200_Delay_us(unsigned int us)
{
    while (us--)
        MAP_UtilsDelay(11);
}
