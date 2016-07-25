/*
 * Author:
 * Copyright (c) 2015 Intel Corporation.
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

#include <string.h>
#include <stdlib.h>

#include "slide.h"
#include "upm_fti.h"

/** 
 * This file implements the Function Table Interface (FTI) for this sensor
 */

const char upm_slide_name[] = "SLIDE";
const char upm_slide_description[] = "Analog slide sensor";
const upm_protocol_t upm_slide_protocol[] = {UPM_ANALOG};
const upm_sensor_t upm_slide_category[] = {UPM_VOLTAGE};

// forward declaration
const void* upm_slide_get_ft(upm_sensor_t sensor_type);
void* upm_slide_init_str(const char* protocol, const char* params);
void upm_slide_close(void* dev);
const upm_sensor_descriptor_t upm_slide_get_descriptor();
upm_result_t upm_slide_set_offset(const void* dev, float offset);
upm_result_t upm_slide_set_scale(const void* dev, float scale);
upm_result_t upm_slide_get_value(const void* dev, float *value);


/* This sensor implementes 2 function tables */
/* 1. Generic base function table */
static const upm_sensor_ft ft_gen =
{
    .upm_sensor_init_name = &upm_slide_init_str,
    .upm_sensor_close = &upm_slide_close,
    .upm_sensor_get_descriptor = &upm_slide_get_descriptor
};

/* 2. VOLTAGE function table */
static const upm_voltage_ft ft_voltage =
{
    .upm_voltage_set_offset = &upm_slide_set_offset,
    .upm_voltage_set_scale = &upm_slide_set_scale,
    .upm_voltage_get_value = &upm_slide_get_value
};

const void* upm_slide_get_ft(upm_sensor_t sensor_type)
{
    switch(sensor_type)
    {
        case UPM_SENSOR:
            return &ft_gen;
        case UPM_VOLTAGE:
            return &ft_voltage;
        default:
            return NULL;
    }
}

void* upm_slide_init_str(const char* protocol, const char* params)
{
    fprintf(stderr, "String initialization - not implemented, using ain0: %s\n", __FILENAME__);
    return slide_init(0);
}

void upm_slide_close(void* dev)
{
    slide_close((slide_context)dev);
}

const upm_sensor_descriptor_t upm_slide_get_descriptor()
{
    /* Fill in the descriptor */
    upm_sensor_descriptor_t usd;
    usd.name = upm_slide_name;
    usd.description = upm_slide_description;
    usd.protocol_size = 1;
    usd.protocol = upm_slide_protocol;
    usd.category_size = 1;
    usd.category = upm_slide_category;

    return usd;
}

upm_result_t upm_slide_set_offset(const void* dev, float offset)
{
    return slide_set_offset((slide_context)dev, offset);
}

upm_result_t upm_slide_set_scale(const void* dev, float scale)
{
    return slide_set_scale((slide_context)dev, scale);
}

upm_result_t upm_slide_get_value(const void* dev, float *value)
{
    return slide_get_value((slide_context)dev, value);
}
