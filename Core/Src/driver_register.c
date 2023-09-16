#include "driver_register.h"
#include "spi.h"

uint8_t ReadFaultSummary()
{
    return (uint8_t)(SPI_Read(0x1));
}
uint8_t ReadFault()
{
    // read status1
    return (uint8_t)(SPI_Read(0x2));
}
void ClearFault()
{
    SPI_Write(8, 0b10001001);
}
