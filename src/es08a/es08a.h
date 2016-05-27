/*
 * Author: Yevgeniy Kiveisha <yevgeniy.kiveisha@intel.com>
 * Modified: Abhishek Malik <abhishek.malik@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ES08A_H_
#define ES08A_H_
#pragma once

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <mraa/pwm.h>
#include "upm_servo.h"

/**
 * @library servo
 * @sensor es08a
 * @comname ES08A Servo
 * @altname Grove Servo
 * @type servos
 * @man emax
 * @web http://www.seeedstudio.com/wiki/Grove_-_Servo
 * @con pwm
 * @kit gsk
 *
 * @brief API for the ES08A Servo
 *
 * This module defines the ES08A interface for ES08A servos.
 * Like other servos, the ES08A servo has a shaft that can be controlled
 * by setting the desired angle. There are also routines for setting
 * and getting the minimum and maximum pulse width as well as the
 * maximum period.
 *
 * @image html es08a.jpg
 * @snippet es08a.c Interesting
 */

#define UPM_ES08A_MIN_PULSE_WIDTH             600
#define UPM_ES08A_MAX_PULSE_WIDTH             2200
#define UPM_ES08A_PERIOD                      20000
#define UPM_ES08A_MAX_ANGLE		            180.0

#define UPM_ES08A_HIGH                        1
#define UPM_ES08A_LOW                         0

/*
 * Opaque pointer to the servo motor struct
 */
//struct _upm_es08a;
typedef struct _upm_es08a* UpmES08A;

/**
 * Instantiates a the servo at the given pin
 *
 * @param pin Servo pin number
 * @param minPulseWidth Minimum pulse width, in microseconds
 * @param maxPulseWidth Maximum pulse width, in microseconds
 * @param waitAndDisablePwm If 1, PWM is enabled only during the setAngle() execution
 * for a period of 1 second, and then turned back off. If 0, PWM remains on afterward.
 */
//void* upm_es08a_init(int32_t pin, int32_t min_pulse_width, int32_t max_pulse_width);
void* upm_es08a_init(int num,...);

/**
 * Halts PWM for this servo and allows it to move freely.
 */
upm_result_t upm_es08a_halt(void* dev);

/**
 * Sets the angle of the servo engine.
 *
 * @param angle Number between 0 and 180
 * @return 0 if successful, non-zero otherwise
 */
upm_result_t upm_es08a_set_angle(void* dev, int32_t angle);

/*
 * Calculating relative pulse time to the value.
 * */
upm_result_t upm_es08a_calc_pulse_travelling(void* dev, int32_t* ret_val, int32_t value);

/**
 * Sets the minimum pulse width
 *
 * @param width Minimum HIGH signal width
 */
void upm_es08a_set_min_pulse_width (void* dev, int width);

/**
 * Sets the maximum pulse width
 *
 * @param width Maximum HIGH signal width
 */
void upm_es08a_set_max_pulse_width (void* dev, int width);

/**
 * Returns the minimum pulse width
 *
 * @return Minimum pulse width
 */
int upm_es08a_get_min_pulse_width (void* dev);

/**
 * Returns the maximum pulse width
 *
 * @return Maximum pulse width
 */
int upm_es08a_get_max_pulse_width (void* dev);

void upm_es08a_get_sensor_info();

//struct _upm_servo_ft upm_es08a_get_function_table();

#endif /* ES08A_H_ */

