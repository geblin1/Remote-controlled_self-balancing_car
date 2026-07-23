#ifndef __NRF24L01_H
#define __NRF24L01_H
#include "NRF24L01_Define.h"
#include <stdint.h>
extern uint8_t NRF24L01_RxPacket[];
extern uint8_t NRF24L01_TxPacket[];
void NRF24L01_W_CSN(uint8_t BitValue);
void NRF24L01_GPIO_Init();
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_Init();
void NRF24L01_Send();
uint8_t NRF24L01_Receive();
#endif
