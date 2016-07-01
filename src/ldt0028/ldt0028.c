/*
 * Author:  Sisinty Sasmita Patra <sisinty.s.patra@intel.com>
 *
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

#include "ldt0028.h"

struct _upm_ldt0028{
	mraa_aio_context aio;
};

const char upm_ldt0028_name[] = "LDT0028";
const char upm_ldt0028_description[] = "Grove Piezo Vibration Sensor";
const upm_protocol_t upm_ldt0028_protocol[] = {UPM_ANALOG};
const upm_protocol_t upm_ldt0028_category[] = {UPM_VIBRATION};

const upm_sensor_descriptor_t upm_ldt0028_get_descriptor(void* dev) {
	upm_sensor_descriptor_t usd;
	usd.name = upm_ldt0028_name;
	usd.description = upm_ldt0028_description;
	usd.protocol_size = 1;
	usd.protocol = upm_ldt0028_protocol;
	usd.category_size = 1;
	usd.category = upm_ldt0028_category;
	return usd;
}

void* upm_ldt0028_get_ft(upm_sensor_t sensor_type) {

	if(sensor_type == UPM_SENSOR) {
		struct _upm_sensor_ft *ft = malloc(sizeof(*ft));
		//ft->upm_sensor_init_name = upm_ldt0028_init_name;
		ft->upm_sensor_close = upm_ldt0028_close;
		ft->upm_sensor_read = upm_ldt0028_read;
		ft->upm_sensor_write = upm_ldt0028_write;
		return ft;
	}

	if(sensor_type == UPM_VIBRATION) {
		struct _upm_vibration_ft *vft = malloc(sizeof(*vft));
		if(vft == NULL){
			printf("Unable to assign memory");
			return NULL;
		}
		vft->upm_vibration_get_value = upm_ldt0028_get_value;
		return vft;
		}
	return NULL;
}

//void* upm_ldt0028_init_name(...);

void* upm_ldt0028_init(int pin)
{
	upm_ldt0028 dev = (upm_ldt0028) malloc(sizeof(struct _upm_ldt0028));

	if(dev == NULL) return NULL;

	dev->aio = mraa_aio_init(pin);

	if(dev->aio == NULL)
	{
		free(dev);
		return NULL;
	}
	return dev;
}

void upm_ldt0028_close(void* dev)
{
	upm_ldt0028 device = (upm_ldt0028) dev;
	mraa_aio_close(device->aio);

	free(dev);
}

upm_result_t upm_ldt0028_read (void* dev, void* value, int len)
{
	upm_ldt0028 device = (upm_ldt0028) dev;

	*(int*)value = mraa_aio_read(device->aio);

	return UPM_SUCCESS;
}

upm_result_t upm_ldt0028_write(void* dev, void* value, int len)
{
	return  UPM_ERROR_NOT_SUPPORTED;
}

upm_result_t upm_ldt0028_get_value(void* dev, float* vibval)
{
	float val;

	upm_ldt0028 device = (upm_ldt0028) dev;
	upm_ldt0028_read(device, &val, 0);

	return UPM_SUCCESS;
}