
#include "MT6816.h"

MT6816 encoder;


void initMT6816(MT6816 *encoder,SPI_HandleTypeDef *hspi)
{
	encoder->angle=0;
	encoder->hspi=hspi;
	encoder->txbuf[0]=(0x80|ANGLE_ADDH);
	encoder->txbuf[1]=(0x80|ANGLE_ADDL);
        HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_SET);
}

void updateMT6816(MT6816 *encoder)
{
	
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(encoder->hspi,encoder->txbuf,1,HAL_MAX_DELAY);	
	
	HAL_SPI_Receive(encoder->hspi,encoder->rxbuf,1,HAL_MAX_DELAY);	
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(encoder->hspi,encoder->txbuf+1,1,HAL_MAX_DELAY);	
	
	HAL_SPI_Receive(encoder->hspi,encoder->rxbuf+1,1,HAL_MAX_DELAY);	
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_SET);
	
	uint8_t angleH=encoder->rxbuf[0];
	uint8_t angleL=encoder->rxbuf[1]&0xFC;
	encoder->angle=(angleH<<8|angleL)>>2;
	encoder->angle_f=(encoder->angle*360.0f/16384);

	
}

uint16_t readMT6816(MT6816 *encoder)
{
	return encoder->angle;
}

