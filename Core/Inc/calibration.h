/*
 * calibration.h
 *
 *  Created on: Mar 25, 2025
 *      Author: Kuprin_IV
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

#define VOLTAGE_MIN_VAL			0
#define VOLTAGE_MAX_VAL			2650
#define VOLTAGE_MAX_INDEX		530

#define CURRENT_MIN_VAL			0
#define CURRENT_MAX_VAL			4095
#define CURRENT_MAX_INDEX		515

extern const uint16_t current_set_10ma_step[];
extern const uint16_t voltage_set_50mv_step[];

#endif /* INC_CALIBRATION_H_ */
