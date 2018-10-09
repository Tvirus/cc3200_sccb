#include "cc3200_sccb.h"
#include "gpio.h"
#include "pin.h"
#include "cc3200_delay.h"




#define SCCB_ID 0x60

//速率100k(最大400)，周期10us
#define  SCCB_PERIOD     10
#define  SCCB_PERIOD_2    5
#define  SCCB_PERIOD_4    3

#define  SCCB_SDL_CLOCK  PRCM_GPIOA0   /* io口时钟 */
#define  SCCB_SDA_CLOCK  PRCM_GPIOA0   /* io口时钟 */
#define  SCCB_SDL_PORT   GPIOA0_BASE
#define  SCCB_SDL_BIT    0x40          /* GPIO6 */
#define  SCCB_SDL_PIN    PIN_61
#define  SCCB_SDA_PORT   GPIOA0_BASE
#define  SCCB_SDA_BIT    0x80          /* GPIO7 */
#define  SCCB_SDA_PIN    PIN_62







static inline void SetSDL(unsigned char x)
{
    if (x)
        MAP_GPIOPinWrite(SCCB_SDL_PORT, SCCB_SDL_BIT, SCCB_SDL_BIT);
    else
        MAP_GPIOPinWrite(SCCB_SDL_PORT, SCCB_SDL_BIT, 0);
}


static inline void SetSDA_IN(void)
{
    MAP_GPIODirModeSet(SCCB_SDA_PORT, SCCB_SDA_BIT, GPIO_DIR_MODE_IN);
    MAP_PinConfigSet(SCCB_SDA_PIN, PIN_STRENGTH_2MA, PIN_TYPE_STD_PU); /* sda数据线读数据必须有内部或者外部上拉 */
}
static inline void SetSDA_OUT(void)
{
    MAP_GPIODirModeSet(SCCB_SDA_PORT, SCCB_SDA_BIT, GPIO_DIR_MODE_OUT);
    MAP_PinConfigSet(SCCB_SDA_PIN, PIN_STRENGTH_2MA, PIN_TYPE_STD);
}
static inline void SetSDA(unsigned char x)
{
    if (x)
        MAP_GPIOPinWrite(SCCB_SDA_PORT, SCCB_SDA_BIT, SCCB_SDA_BIT);
    else
        MAP_GPIOPinWrite(SCCB_SDA_PORT, SCCB_SDA_BIT, 0);
}




void SCCB_Init(void)
{
    /* 使能时钟 */
    MAP_PRCMPeripheralClkEnable(SCCB_SDL_CLOCK, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(SCCB_SDA_CLOCK, PRCM_RUN_MODE_CLK);

    MAP_PinTypeGPIO(SCCB_SDL_PIN, PIN_MODE_0, FALSE); /* 推挽 */
    MAP_PinTypeGPIO(SCCB_SDA_PIN, PIN_MODE_0, FALSE); /* 推挽 */

    SetSDA_OUT();
    SetSDA(1);
    MAP_GPIODirModeSet(SCCB_SDL_PORT, SCCB_SDL_BIT, GPIO_DIR_MODE_OUT);
    SetSDL(1);
}


static void Start(void)
{
    SetSDA(1);
    SetSDL(1);
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDA(0);
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDL(0);
    CC3200_Delay_us(SCCB_PERIOD_4);
}


static void Stop(void)
{
    SetSDA(0);
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDL(1);
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDA(1);
    CC3200_Delay_us(SCCB_PERIOD_4);
}



static void WriteByte(unsigned char ucDate)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        SetSDA(ucData & 0x80);
        ucData <<= 1;
        CC3200_Delay_us(SCCB_PERIOD_4);
        SetSDL(1);
        CC3200_Delay_us(SCCB_PERIOD_2);
        SetSDL(0);
        CC3200_Delay_us(SCCB_PERIOD_4);
    }

    SetSDA(1);
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDL(1);
    CC3200_Delay_us(SCCB_PERIOD_2);
    SetSDL(0);
    CC3200_Delay_us(SCCB_PERIOD_4);
}



static unsigned char ReadByte(void)
{
    unsigned char i, ucData = 0;

    SetSDA_IN();

    for (i = 0; i < 8; i++)
    {
        ucData <<= 1;
        CC3200_Delay_us(SCCB_PERIOD_4);
        SetSDL(1);
        CC3200_Delay_us(SCCB_PERIOD_2);
        if (MAP_GPIOPinRead(SCCB_SDA_PORT, SCCB_SDA_BIT) & SCCB_SDA_BIT)
        {
            ucData |= 0x01;
        }

        SetSDL(0);
        CC3200_Delay_us(SCCB_PERIOD_4);
    }

    SetSDA_OUT();

    SetSDA(1); /* 这里必须是1，否则会有读取数据错误或者一直重复的问题 */
    CC3200_Delay_us(SCCB_PERIOD_4);
    SetSDL(1);
    CC3200_Delay_us(SCCB_PERIOD_2);
    SetSDL(0);
    CC3200_Delay_us(SCCB_PERIOD_4);
    
    return ucData;
}


void SCCB_WriteReg(unsigned char ucReg,unsigned char ucData)
{
    Start();
    WriteByte(SCCB_ID & 0xfe);
    WriteByte(ucReg);
    WriteByte(ucData);
    Stop();
}


unsigned char SCCB_ReadReg(unsigned char ucReg)
{
    unsigned char ucData;

    Start();
    WriteByte(SCCB_ID & 0xfe);
    WriteByte(ucReg);
    Stop();

    Start();
    WriteByte(SCCB_ID | 0x01);
    ucData = ReadByte();
    Stop();

    return ucData;
}
