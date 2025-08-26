#ifndef __W25Q128_H
#define __W25Q128_H
#include <stdint.h>

void W25Q128_Init(void);

uint8_t W25Q128_SectorErase(uint32_t address);
uint8_t W25Q128_ChipErase(void);
uint8_t W25Q128_PageProgram(uint32_t address,uint8_t *Arry,uint16_t len);

void W25Q128_Read_id(uint8_t *MID,uint16_t *DID);
void W25Q128_ReadData(uint32_t address,uint8_t *Arry,uint16_t len);

#endif // !__W25Q128_H
