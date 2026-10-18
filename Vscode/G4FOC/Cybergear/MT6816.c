#include "MT6816.h"





void initMT6816(MT6816 *encoder,SPI_HandleTypeDef *hspi)
{
	encoder->angle=0.0f;
	encoder->hspi=hspi;
	encoder->txbuf[0]=(0x80|ANGLE_ADDH);
	encoder->txbuf[1]=(0x80|ANGLE_ADDL);
}

void updateMT6816(MT6816 *encoder)
{
	
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_RESET);	
	HAL_SPI_TransmitReceive(encoder->hspi,encoder->txbuf,encoder->rxbuf,1,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(encoder->hspi,encoder->txbuf+1,encoder->rxbuf+1,1,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(MT6816_CS_GPIO_Port,MT6816_CS_Pin,GPIO_PIN_SET);

	encoder->angle=((encoder->rxbuf[0]&0x00FF)<<8)|(encoder->rxbuf[1]&0x00FF);

	
}

uint16_t readMT6816(MT6816 *encoder)
{
	return encoder->angle;
}

//int16_t ENC_CalcAngle( ENCODER_Handle_t * pHandle )
//{
//	updateMT6816(&encoder);
//	return encoder.angle;
//}
