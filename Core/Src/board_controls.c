/*
 * board_controls.c
 *
 *  Created on: 29 дек. 2023 г.
 *      Author: Kuprin_IV
 */

#include "board_controls.h"

static uint8_t is_initial_state_got = 0;
static EncoderSignalsState enc1_signals = {0, 0, 1, 0, 0, 0x7FFF, 0, 0, 1, 0x7FFF}, enc2_signals = {0, 0, 1, 0, 0, 0x7FFF, 0, 0, 1, 0x7FFF};

static uint8_t majorFilter(EncoderSignalsState* ec1, EncoderSignalsState* ec2);
static void updateEncoderCounter(EncoderSignalsState* ec_signals);
static uint8_t getEncoderState(EncoderSignalsState* ec_signals, EncoderState* ec_state);

/**
  * @brief  Scan encoders state
  * @params  ec1, ec2- data structures with encoders state
  * @retval 0 - encoders state isn't changed, 1 - encoders state is changed
  */
uint8_t scanEncoders(EncoderState* ec1, EncoderState* ec2)
{
	uint8_t is_encoders_state_changed = 0;
	uint8_t is_state_ready = 0;
	static uint16_t state_ticks_cntr;

	// get filtered encoders signals state
	is_state_ready = majorFilter(&enc1_signals, &enc2_signals);

	if(is_state_ready)
	{
		if(!is_initial_state_got)
		{
			is_initial_state_got = 1;

			// initialize previous encoders signals state
			enc1_signals.a_pin_state_prev = enc1_signals.a_pin_state;
			enc1_signals.b_pin_state_prev = enc1_signals.b_pin_state;
			enc1_signals.btn_state_prev = enc1_signals.btn_state;

			enc2_signals.a_pin_state_prev = enc2_signals.a_pin_state;
			enc2_signals.b_pin_state_prev = enc2_signals.b_pin_state;
			enc2_signals.btn_state_prev = enc2_signals.btn_state;
		}
		else
		{
			// update encoders counter
			updateEncoderCounter(&enc1_signals);
			updateEncoderCounter(&enc2_signals);

			// get encoders state with "STATE_SCAN_PERIOD_MS"
			if(state_ticks_cntr++ >= STATE_SCAN_TICKS)
			{
				getEncoderState(&enc1_signals, ec1);
				getEncoderState(&enc2_signals, ec2);

				is_encoders_state_changed = 1;
				state_ticks_cntr = 0; // reset tick counter
			}
		}
	}

	return is_encoders_state_changed;
}

/**************** static functions ***************************/
/**
  * @brief  Make majority filter of encoder signals
  * @params  ec1, ec2 - data structures with encoders signals state
  * @retval 0 - output data isn't ready, 1 - output data is ready
  */
static uint8_t majorFilter(EncoderSignalsState* ec1, EncoderSignalsState* ec2)
{
	static uint8_t ticks_cntr;
	static uint8_t ec1a_cntr, ec1b_cntr, ec1_btn_cntr;
	static uint8_t ec2a_cntr, ec2b_cntr, ec2_btn_cntr;
	uint8_t is_ready = 0;

	// get encoder input pins state
	if(EC1A_GPIO_Port->IDR & EC1A_Pin) ec1a_cntr++;
	if(EC1B_GPIO_Port->IDR & EC1B_Pin) ec1b_cntr++;
	if(EC1_Btn_GPIO_Port->IDR & EC1_Btn) ec1_btn_cntr++;

	if(EC2A_GPIO_Port->IDR & EC2A_Pin) ec2a_cntr++;
	if(EC2B_GPIO_Port->IDR & EC2B_Pin) ec2b_cntr++;
	if(EC2_Btn_GPIO_Port->IDR & EC2_Btn) ec2_btn_cntr++;

	if(ticks_cntr++ < MAJOR_FILT_AVG-1)
	{
		is_ready = 0;
	}
	else
	{
		// set encoders state
		ec1->a_pin_state = (ec1a_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;
		ec1->b_pin_state = (ec1b_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;
		ec1->btn_state = (ec1_btn_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;

		ec2->a_pin_state = (ec2a_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;
		ec2->b_pin_state = (ec2b_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;
		ec2->btn_state = (ec2_btn_cntr > MAJOR_FILT_AVG/2+1) ? 1 : 0;

		// reset counters
		ec1a_cntr = ec1b_cntr = ec1_btn_cntr = 0;
		ec2a_cntr = ec2b_cntr = ec2_btn_cntr = 0;
		ticks_cntr = 0;

		// set ready flag
		is_ready = 1;
	}
	return is_ready;
}

/**
  * @brief Update encoder counter from signals state
  * @param  ec_signals - data structure with encoder signals state
  * @retval none
  */
static void updateEncoderCounter(EncoderSignalsState* ec_signals)
{
	// detect A signal rising edge
	if(ec_signals->a_pin_state == 1 && ec_signals->a_pin_state_prev == 0)
	{
		// detect turn direction by A and B signal level
		if(ec_signals->b_pin_state == ec_signals->b_pin_state_prev)
		{
			if(ec_signals->a_pin_state == ec_signals->b_pin_state)
			{
				ec_signals->counter--;
			}
			else
			{
				ec_signals->counter++;
			}
		}
	}

	// update encoders signals previous state
	ec_signals->a_pin_state_prev = ec_signals->a_pin_state;
	ec_signals->b_pin_state_prev = ec_signals->b_pin_state;
}

/**
  * @brief Get encoder state from signals
  * @param  ec_signals - data structure with encoder signals state
  * @param  ec_state - resulted state of encoder
  * @retval 0 - encoder state isn't changed, 1 - encoder state is changed
  */
static uint8_t getEncoderState(EncoderSignalsState* ec_signals, EncoderState* ec_state)
{
	ButtonState ec_btn_prev_state = ec_state->btn_state;

	// get encoder button state
	if(ec_signals->btn_state == 0 && ec_signals->btn_state_prev == 1) // button is pressed
	{
		ec_state->btn_state = Pressed;
	}
	else if(ec_signals->btn_state == 0 && ec_signals->btn_state_prev == 0) // button is holding on
	{
		if(ec_signals->long_btn_press_cntr < LONG_PRESS_TICKS)
		{
			ec_signals->long_btn_press_cntr++;
		}
		else if(!ec_signals->is_long_btn_press_detected)
		{
			ec_signals->is_long_btn_press_detected = 1;
			ec_state->btn_state = LongPressed;
		}
	}
	else
	{
		ec_state->btn_state = NotPressed;

		// reset button long press tick counter and long press detection flag
		ec_signals->long_btn_press_cntr = 0;
		ec_signals->is_long_btn_press_detected = 0;
	}

	// get encoder counter offset
	ec_state->counter_offset = (int8_t)(ec_signals->counter - ec_signals->counter_prev);

	// check is encoder state changed
	ec_state->is_state_changed = ((ec_state->counter_offset != 0) || (ec_state->btn_state != ec_btn_prev_state));

	// update counter and button previous state
	ec_signals->counter_prev = ec_signals->counter;
	ec_signals->btn_state_prev = ec_signals->btn_state;

	return ec_state->is_state_changed;
}
