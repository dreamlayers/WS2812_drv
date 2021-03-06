#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "WS2812_drv.h"

#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"

void
WStoSPI(uint8_t *pi8SPIData, uint8_t ui8Color)
{
#if WS2812_SPI_BIT_WIDTH == 8
    //
    //8-bit implementation
    //
    int i;
    for(i=0;i<8;i++)
    {
        if(ui8Color & (0x80 >> i))
        {
            pi8SPIData[i] = WS2812_SPI_HIGH;
        }
        else
        {
            pi8SPIData[i] = WS2812_SPI_LOW;
        }
    }
#elif WS2812_SPI_BIT_WIDTH == 4
    //
    //4-bit implementation
    //
    uint8_t mask = 0x80;
    volatile uint8_t *dest = (volatile uint8_t *)pi8SPIData;
    do {
        uint8_t val;
        val = (ui8Color & mask) ? (WS2812_SPI_HIGH << 4) :
                                    WS2812_SPI_LOW << 4;
        mask >>= 1;
        val |= (ui8Color & mask) ? WS2812_SPI_HIGH : WS2812_SPI_LOW;
        mask >>= 1;
        *(dest++) = val;
    } while (mask != 0);
#else
#error Unsupported WS2812_SPI_BIT_WIDTH
#endif

}

void
WSGRBtoSPI(uint8_t *pi8SPIData, uint8_t ui8Green, uint8_t ui8Red,
           uint8_t ui8Blue)
{
    WStoSPI(pi8SPIData+(WS2812_SPI_BIT_WIDTH*WS2812_GREEN_OFFS), ui8Green);
    WStoSPI(pi8SPIData+(WS2812_SPI_BIT_WIDTH*WS2812_RED_OFFS), ui8Red);
    WStoSPI(pi8SPIData+(WS2812_SPI_BIT_WIDTH*WS2812_BLUE_OFFS), ui8Blue);
}

void
WSSetLEDColors(uint8_t *pi8SPILEDs, uint16_t ui16LED, uint8_t ui8Green,
               uint8_t ui8Red, uint8_t ui8Blue)
{
    WStoSPI(pi8SPILEDs + (WS2812_SPI_BYTE_PER_CLR*WS2812_SPI_BIT_WIDTH*ui16LED)
            + (WS2812_SPI_BIT_WIDTH * WS2812_GREEN_OFFS), ui8Green);
    WStoSPI(pi8SPILEDs + (WS2812_SPI_BYTE_PER_CLR*WS2812_SPI_BIT_WIDTH*ui16LED)
            + (WS2812_SPI_BIT_WIDTH * WS2812_RED_OFFS), ui8Red);
    WStoSPI(pi8SPILEDs + (WS2812_SPI_BYTE_PER_CLR*WS2812_SPI_BIT_WIDTH*ui16LED)
            + (WS2812_SPI_BIT_WIDTH * WS2812_BLUE_OFFS), ui8Blue);
}

void
WSArrayInit(uint8_t *pi8SPIData, uint16_t ui16Len)
{
    int i;
    
    if(ui16Len == 0)
    {
        return;
    }
    
    for(i=0;i<ui16Len;i++)
    {
        pi8SPIData[i] = WS2812_SPI_LOW;
        //
        // 4 bit implementation
        //
        //pi8SPIData[i] = WS2812_SPI_LOW | (WS2812_SPI_LOW << 4);
    }
}
