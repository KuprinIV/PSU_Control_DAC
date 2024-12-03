/*
 * board_controls.h
 *
 *  Created on: 29 дек. 2023 г.
 *      Author: Kuprin_IV
 */

#ifndef INC_BOARD_CONTROLS_H_
#define INC_BOARD_CONTROLS_H_

#include "stm32f0xx_hal.h"
#include "main.h"

#define MAJOR_FILT_AVG				5
#define STATE_SCAN_PERIOD_MS		20
#define STATE_SCAN_TICKS			((STATE_SCAN_PERIOD_MS*1000)/(SCAN_PERIOD_US*MAJOR_FILT_AVG))
#define LONG_PRESS_TICKS			(2000/STATE_SCAN_PERIOD_MS) // 2 sec

typedef enum {NotPressed = 0, Pressed = 1, LongPressed = 2} ButtonState;

typedef struct
{
	uint8_t a_pin_state;
	uint8_t b_pin_state;
	uint8_t btn_state;
	uint16_t long_btn_press_cntr;
	uint8_t is_long_btn_press_detected;
	uint16_t counter;

	uint8_t a_pin_state_prev;
	uint8_t b_pin_state_prev;
	uint8_t btn_state_prev;
	uint16_t counter_prev;
}EncoderSignalsState;

typedef struct
{
	int8_t counter_offset;
	ButtonState btn_state;
	uint8_t is_state_changed;
}EncoderState;

uint8_t scanEncoders(EncoderState* ec1, EncoderState* ec2);
void sendEncodersState(EncoderState* ec1, EncoderState* ec2);

#endif /* INC_BOARD_CONTROLS_H_ */
