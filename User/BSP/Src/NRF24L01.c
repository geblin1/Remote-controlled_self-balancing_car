/*
 * @Author: geblin1 3390931275@qq.com
 * @Date: 2026-07-20 17:30:24
 * @LastEditors: geblin1 3390931275@qq.com
 * @LastEditTime: 2026-07-20 19:49:55
 * @FilePath: \NRF24L01无线通信模块\User\BSP\Src\NRF24L01.c
 * @Description: NRF24L01无线通信模块的驱动
 */
#include "gpio.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
#include "NRF24L01_Define.h"
#define NRF24L01_RX_PACKET_WIDTH    32
#define NRF24L01_TX_PACKET_WIDTH    32
uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH];
uint8_t NRF24L01_TxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];
uint8_t NRF24L01_RxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
void NRF24L01_W_CE(uint8_t BitValue){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, (GPIO_PinState)BitValue);
}
void NRF24L01_W_CSN(uint8_t BitValue){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (GPIO_PinState)BitValue);
}
void NRF24L01_W_SCK(uint8_t BitValue){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (GPIO_PinState)BitValue);
}
void NRF24L01_W_MOSI(uint8_t BitValue){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (GPIO_PinState)BitValue);
}
GPIO_PinState NRF24L01_R_MISO(){
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
}
void NRF24L01_GPIO_Init(){
    NRF24L01_W_CE(0);
    NRF24L01_W_CSN(1);
    NRF24L01_W_SCK(0);
    NRF24L01_W_MOSI(0);
}
/*通信协议*/
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte){
    for(uint8_t i = 0; i < 8; i++){
        /*SPI移出数据*/
        if(Byte & 0x80){
            NRF24L01_W_MOSI(1);
        }
        else{
            NRF24L01_W_MOSI(0);
        }
        Byte <<= 1;
        /*SCK置高电平*/
        NRF24L01_W_SCK(1);
        /*SPI移入数据*/
        if(NRF24L01_R_MISO()){
            Byte |= 1;
        }
        /*SCK置低电平*/
        NRF24L01_W_SCK(0);
    }
    return Byte;
}
/*指令实现*/
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    NRF24L01_SPI_SwapByte(Data);
    NRF24L01_W_CSN(1);
}
uint8_t NRF24L01_ReadReg(uint8_t RegAddress){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    uint8_t Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
    return Data;
}

void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    for(uint8_t i = 0; i < Count; i++){
        NRF24L01_SPI_SwapByte(DataArray[i]);
    }
    NRF24L01_W_CSN(1);
}
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    for(uint8_t i = 0; i < Count; i++){
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    }
    NRF24L01_W_CSN(1);
}

void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
    for(uint8_t i = 0; i < Count; i++){
        NRF24L01_SPI_SwapByte(DataArray[i]);
    }
    NRF24L01_W_CSN(1);
}
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
    for(uint8_t i = 0; i < Count; i++){
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    }
    NRF24L01_W_CSN(1);
}

void NRF24L01_FlushTx(){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
    NRF24L01_W_CSN(1);
}
void NRF24L01_FlushRx(){
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
    NRF24L01_W_CSN(1);
}

uint8_t NRF24L01_ReadStatus(){
    uint8_t Status;
    NRF24L01_W_CSN(0);
    Status = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
    return Status;
}
/*功能函数*/
void NRF24L01_PowerDown(){
    uint8_t Config;
    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    Config &= ~0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}
void NRF24L01_StandbyI(){
    uint8_t Config;
    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    Config |= 0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}
void NRF24L01_RxMode(){
    uint8_t Config;
    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    Config |= 0x03;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
    NRF24L01_W_CE(1);
}
void NRF24L01_TxMode(){
    uint8_t Config;
    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    Config |= 0x02;
    Config &= ~0x01;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
    NRF24L01_W_CE(1);
}

void NRF24L01_Init(){
    NRF24L01_GPIO_Init();
    NRF24L01_WriteReg(NRF24L01_CONFIG, 0x08);
    NRF24L01_WriteReg(NRF24L01_EN_AA, 0x3F);
    NRF24L01_WriteReg(NRF24L01_EN_RXADDR, 0x01);
    NRF24L01_WriteReg(NRF24L01_SETUP_AW, 0x03);
    NRF24L01_WriteReg(NRF24L01_SETUP_RETR, 0x03);
    NRF24L01_WriteReg(NRF24L01_RF_CH, 0x02);
    NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x0E);

    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);

    NRF24L01_WriteReg(NRF24L01_RX_PW_P0, NRF24L01_RX_PACKET_WIDTH);

    NRF24L01_RxMode();
}
void NRF24L01_Send(){
    uint8_t Status;

    NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);
    NRF24L01_WriteTxPayload(NRF24L01_TxPacket, NRF24L01_TX_PACKET_WIDTH);
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);

    NRF24L01_TxMode();

    while(1){
        Status = NRF24L01_ReadStatus();
        if(Status & 0x20){
            /*发送成功*/
            break;
        }
        else if(Status & 0x10){
            /*发送失败*/
            break;
        }
    }
    NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);
    NRF24L01_FlushTx();
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);

    NRF24L01_RxMode();
}
uint8_t NRF24L01_Receive(){
    uint8_t Status;
    Status = NRF24L01_ReadStatus();
    if(Status & 0x40){
        NRF24L01_ReadRxPayload(NRF24L01_RxPacket, NRF24L01_RX_PACKET_WIDTH);
        NRF24L01_WriteReg(NRF24L01_STATUS, 0x40);
        NRF24L01_FlushRx();
        return 1;
    }
    return 0;
}
