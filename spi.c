/*
 * spi.c
 *
 *  Created on: 25 maj 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/spi.h"





void SPI_init(void)
{
	//	-initialization procedure,


	//	Enable clock for SPI:
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

	LL_SPI_Disable(SPI1);

	//	Set operation mode: MODE_MASTER, MODE_SLAVE,
	LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

	//	Set transfer direction mode: FULL_DUPLEX, SIMPLEX_RX, HALF_DUPLEX_RX, HALF_DUPLEX_TX,
	LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);

	//	Set clock polarity: POLARITY_LOW, POLARITY_HIGH,
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);

	//	Set clock phase: PHASE_1EDGE, PHASE_2EDGE,
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);

	//	Set NSS mode: NSS_SOFT, NSS_HARD_INPUT, NSS_HARD_OUTPUT,
	LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);

	//	Set baud rate prescaler: LL_SPI_BAUDRATEPRESCALER_DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV128, DIV256,
	LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV256);

	//	Set transfer bit order: LSB_FIRST, MSB_FIRST,
	LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);

	//	Set frame data width: DATAWIDTH_4BIT ... DATAWIDTH_16BIT,
	LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);

	//	Set serial protocol used: PROTOCOL_MOTOROLA, PROTOCOL_TI,
	LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);

	//	Set threshold of RXFIFO that triggers an RXNE event: RX_FIFO_TH_HALF, RX_FIFO_TH_QUARTER,
	LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);

	LL_SPI_Enable(SPI1);
}

uint8_t SPI_send(uint8_t data)
{
	//	SPI is switched off, so we have to switched it back,

	uint8_t data_received = 0;

	//	Enable SPI peripheral:
	//LL_SPI_Enable(SPI1);

	//	Waiting for the TXE flag being set. It means we can start sending data.
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));

	//	Send some data:
	LL_SPI_TransmitData8(SPI1, data);

	//	Waiting before all data are send from TX_FIFO. TX_FIFO is empty,
	while(LL_SPI_GetTxFIFOLevel(SPI1) != LL_SPI_TX_FIFO_EMPTY);

	//	Waiting for the end of transmision:
	while(LL_SPI_IsActiveFlag_BSY(SPI1) != 0);

	//	Disable SPI peripheral:
	//LL_SPI_Disable(SPI1);

	//	Waiting for the received data and read them:
	while(LL_SPI_GetRxFIFOLevel(SPI1) != LL_SPI_RX_FIFO_EMPTY)
	{
		data_received = LL_SPI_ReceiveData8(SPI1);
	}

	//	Clear OVF flag:
	LL_SPI_ClearFlag_OVR(SPI1);

	return data_received;
}


uint8_t SPI_receive_2(void)
{
	//	SPI is switched off, so we have to switched it back,

	uint8_t data2 = 0;

	//	Enable SPI peripheral:
	//LL_SPI_Enable(SPI1);

	//	Waiting for the TXE flag being set. It means we can start sending data.
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));

	//	Send some data:
	LL_SPI_TransmitData8(SPI1, 0xFF);
	LL_SPI_TransmitData8(SPI1, 0xFF);

	while(LL_SPI_IsActiveFlag_RXNE(SPI1));

	LL_SPI_ReceiveData8(SPI1);

	//	Waiting before all data are send from TX_FIFO. TX_FIFO is empty,
	while(LL_SPI_GetTxFIFOLevel(SPI1) != LL_SPI_TX_FIFO_EMPTY);

	//	Waiting for the end of transmision:
	while(LL_SPI_IsActiveFlag_BSY(SPI1) != 0);

	//	Waiting for the received data and read them:
	while(LL_SPI_GetRxFIFOLevel(SPI1) != LL_SPI_RX_FIFO_EMPTY)
	{
		data2 = LL_SPI_ReceiveData8(SPI1);
	}

	//	Clear OVF flag:
	LL_SPI_ClearFlag_OVR(SPI1);

	return data2;
}



uint8_t SPI_receive(void)
{
	//	SPI is switched off, so we have to switched it back,

	uint8_t data = 0;

	//	Enable SPI peripheral:
	//LL_SPI_Enable(SPI1);

	//	Waiting for the TXE flag being set. It means we can start sending data.
	while(!LL_SPI_IsActiveFlag_TXE(SPI1));

	//	Send some data:
	LL_SPI_TransmitData8(SPI1, 0xFF);

	while(LL_SPI_IsActiveFlag_RXNE(SPI1));

	//	Waiting before all data are send from TX_FIFO. TX_FIFO is empty,
	while(LL_SPI_GetTxFIFOLevel(SPI1) != LL_SPI_TX_FIFO_EMPTY);

	//	Waiting for the end of transmision:
	while(LL_SPI_IsActiveFlag_BSY(SPI1) != 0);

	//	Waiting for the received data and read them:
	while(LL_SPI_GetRxFIFOLevel(SPI1) != LL_SPI_RX_FIFO_EMPTY)
	{
		data = LL_SPI_ReceiveData8(SPI1);
	}

	//	Clear OVF flag:
	LL_SPI_ClearFlag_OVR(SPI1);

	return data;
}

void SPI_set_speed(uint32_t speed)
{
	LL_SPI_Disable(SPI1);

	//	Set baud rate prescaler: LL_SPI_BAUDRATEPRESCALER_DIV2, DIV4, DIV8, DIV16, DIV32, DIV64, DIV128, DIV256,
	LL_SPI_SetBaudRatePrescaler(SPI1, speed);

	LL_SPI_Enable(SPI1);
}



