#ifndef __MT6816_H__
#define __MT6816_H__

#include <stdint.h>
#include "spi.h"
#include "main.h"
#include "gpio.h"

#define ANGLE_ADDH 0x03
#define ANGLE_ADDL 0x04


typedef struct __MT6816__
{
    uint8_t txbuf[2];
    uint8_t rxbuf[2];
    uint16_t angle;
    SPI_HandleTypeDef *hspi;
    

}MT6816;

void initMT6816(MT6816 *encoder,SPI_HandleTypeDef *hspi);
void updateMT6816(MT6816 *encoder);
uint16_t readMT6816(MT6816 *encoder);



#endif
