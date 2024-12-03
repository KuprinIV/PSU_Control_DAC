/*
 * mcp4725.h
 *
 *  Created on: Nov 26, 2024
 *      Author: Ilya
 */

#ifndef SRC_MCP4725_H_
#define SRC_MCP4725_H_

#include "stm32f0xx_hal.h"

typedef enum
{
	DAC_STATUS = 0,
	DAC_VALUE,
	DAC_EEPROM_VALUE
}DacDataType;

uint8_t MCP4725_check(uint8_t dev_address);
void MCP4725_writeDAC_register(uint8_t dev_address, uint16_t dac_value, uint8_t is_save_in_eeprom);
uint16_t MCP4725_readDAC_register_and_EEPROM(uint8_t dev_address, DacDataType data_type);

#endif /* SRC_MCP4725_H_ */
