/*
 * Author: Jon Trulson <jtrulson@ics.com>
 *         Abhishek Malik <abhishek.malik@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * Thanks to Adafruit for supplying a google translated version of the
 * Chinese datasheet and some clues in their code.
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

#include "urm37.h"

struct _upm_urm37v4 {
	mraa_aio_context	aio;
	mraa_gpio_context	gpio_reset_pin;
	mraa_gpio_context	gpio_trig_pin;
	mraa_uart_context	uart;
	uint8_t 			analog_pin;
	uint8_t				reset_pin;
	uint8_t				trigger_pin;
	uint8_t				uart_pin;
	upm_protocol_t		mode;
	int					a_res;
	int 				degrees;
};

void* upm_get_function_table(upm_sensor_t sensor_type){
	if(sensor_type == UPM_TEMPERATURE){
		struct _upm_temperature_ft *tft = malloc(sizeof(*tft));
		if(tft == NULL){
			printf("Unable to assign memory to the Temperature structure");
			return NULL;
		}
		tft->upm_temperature_get_value = upm_urm37_get_temperature;
		return tft;
	}
	else if(sensor_type == UPM_DISTANCE){;
	struct _upm_dist_ft *dft = malloc(sizeof(*dft));
	if(dft == NULL){
		printf("Unable to assign memory to the Distance structure");
		return NULL;
	}
	dft->upm_distance_get_value = upm_urm37_get_distance;
	return dft;
	}
	else if(sensor_type == UPM_SENSOR){
		struct _upm_sensor_ft *sft = malloc(sizeof(*sft));
		if(sft == NULL){
			printf("Unable to assign memory to the Sensor structure");
			return NULL;
		}
		sft->upm_sensor_init_name = upm_urm37_init_name;
		sft->upm_sensor_close = upm_urm37_close;
		sft->upm_sensor_read = upm_urm37_read;
		sft->upm_sensor_write = upm_urm37_write;
	}
	return NULL;
}

void* upm_urm37_init_name(int num,...){
	// analog pin, uart pin, reset pin, trigger pin, mode
	upm_urm37 dev = (upm_urm37) malloc(sizeof(struct _upm_urm37v4));
	va_list pin_list;
	va_start(pin_list, num);
	// read out all the pins from the va list
	dev->analog_pin =  va_arg(pin_list, int);
	dev->uart_pin = va_arg(pin_list, int);
	dev->reset_pin = va_arg(pin_list, int);
	dev->trigger_pin = va_arg(pin_list, int);
	// read out the mode as well to start off with
	dev->mode = va_arg(pin_list, int);

	// initialize the contexts
	// analog part
	dev->aio = mraa_aio_init(dev->analog_pin);
	// both analog and uart
	dev->gpio_reset_pin = mraa_gpio_init(dev->reset_pin);
	dev->gpio_trig_pin = mraa_gpio_init(dev->trigger_pin);
	// uart part
	dev->uart = mraa_uart_init(dev->uart_pin);

	// setting degrees (this needs to be verified)
	dev->degrees = 0;

	// only analog
	dev->a_res = (1 << mraa_aio_get_bit(dev->aio));
	mraa_gpio_dir(dev->gpio_trig_pin, MRAA_GPIO_OUT);
	mraa_gpio_write(dev->gpio_trig_pin, 1);

	// uart only
	// 9600 baud is the only supported baud rate...
	mraa_uart_set_baudrate(dev->uart, 9600);
	mraa_uart_set_non_blocking(dev->uart, FALSE);

	// analog and uart
	mraa_gpio_dir(dev->gpio_reset_pin, MRAA_GPIO_OUT);
	upm_urm37_reset(dev);

	return dev;
}

void* upm_urm37_init(uint8_t a_pin, uint8_t reset_pin, uint8_t trigger_pin, float a_ref, uint8_t uart, upm_protocol_t mode){
	upm_urm37 dev = (upm_urm37) malloc(sizeof(struct _upm_urm37v4));
	dev->analog_pin =  a_pin;
	dev->uart_pin = uart;
	dev->reset_pin = reset_pin;
	dev->trigger_pin = trigger_pin;
	// read out the mode as well to start off with
	dev->mode = mode;

	// initialize the contexts
	// analog part
	dev->aio = mraa_aio_init(dev->analog_pin);
	// both analog and uart
	dev->gpio_reset_pin = mraa_gpio_init(dev->reset_pin);
	dev->gpio_trig_pin = mraa_gpio_init(dev->trigger_pin);
	// uart part
	dev->uart = mraa_uart_init(dev->uart_pin);

	// setting degrees (this needs to be verified)
	dev->degrees = 0;

	// only analog
	dev->a_res = (1 << mraa_aio_get_bit(dev->aio));
	mraa_gpio_dir(dev->gpio_trig_pin, MRAA_GPIO_OUT);
	mraa_gpio_write(dev->gpio_trig_pin, 1);

	// uart only
	// 9600 baud is the only supported baud rate...
	mraa_uart_set_baudrate(dev->uart, 9600);
	mraa_uart_set_non_blocking(dev->uart, FALSE);

	// analog and uart
	mraa_gpio_dir(dev->gpio_reset_pin, MRAA_GPIO_OUT);
	upm_urm37_reset(dev);

	return dev;
}

void upm_urm37_close(void* dev){
	upm_urm37 device = (upm_urm37) dev;
	if(device->aio)
		free(device->aio);
	if(device->gpio_reset_pin)
		free(device->gpio_reset_pin);
	if(device->gpio_trig_pin)
		free(device->gpio_trig_pin);
	if(device->uart)
		free(device->uart);
	free(device);
}

upm_result_t upm_urm37_reset(void* dev){
	upm_urm37 device = (upm_urm37) dev;
	mraa_gpio_write(device->gpio_reset_pin, 0);
	// OS specific : Add to utilities
	usleep(100);
	mraa_gpio_write(device->gpio_reset_pin, 1);
	// OS specific
	// wait for reset to complete
	sleep(3);

	return UPM_SUCCESS;
}

int upm_urm37_data_available(void* dev, uint32_t millis){
	upm_urm37 device = (upm_urm37) dev;
	mraa_boolean_t ret = mraa_uart_data_available(device->uart, millis);
	//*data_avail = ret;
	return ret;
}

upm_result_t upm_urm37_read_data_string(void* dev, uint32_t len, char* data){
	upm_urm37 device = (upm_urm37) dev;
	int v = mraa_uart_read(device->uart, data, (size_t) len);
	printf("success/failure: %d\n", v);
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_write_data_string(void* dev, const char* data_w, int len){
	upm_urm37 device = (upm_urm37) dev;
	mraa_uart_flush(device->uart);
	//printf("send command: %c %c %c %c\n", data_w[0], data_w[1], data_w[2], data_w[3]);
	mraa_uart_write(device->uart, data_w, len);
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_send_command(void* dev, char* cmd, char* response, int len){
	upm_urm37 device = (upm_urm37) dev;
	if(device->mode == 	UPM_ANALOG){
		printf("Temperature measurement not available in analog mode\n");
		return UPM_ERROR_NOT_SUPPORTED;
	}
	int tries = 0;
	//char resp[8];


	while(tries++ < UPM_URM37_MAX_RETRIES){
		upm_urm37_write_data_string(device, cmd, len);
		//mraa_boolean_t ret = FALSE;
		if(!upm_urm37_data_available(device, UPM_URM37_WAIT_TIMEOUT)){
			printf("Time out occurred\n");
			continue;
		}
		/*upm_urm37_data_available(device, UPM_URM37_WAIT_TIMEOUT, &ret);
		if(ret != TRUE){
			printf("Time out occured\n");
			return UPM_ERROR_TIMED_OUT;
		}*/

		// assigning array some space
		int length = 8;
		char resp[length];
		printf("prior: %d %d %d %d %d %d %d %d \n", resp[0],resp[1],resp[2],resp[3],resp[4],resp[5],resp[6],resp[7]);
		sleep(1);
		//		char* resp = (char*) malloc(sizeof(char) * length);
		upm_urm37_read_data_string(device->uart, length, resp);
		printf("returned value: %d %d %d %d %d %d %d %d \n", resp[0],resp[1],resp[2],resp[3],resp[4],resp[5],resp[6],resp[7]);
		if(resp[4] == '\0'){
			printf("Invalid Packet size\n");
			continue;
		}
		else{
			// we have data, verify cksum, return the response if it's
			// good, retry otherwise
			uint8_t cksum = (uint8_t)(resp[0] + resp[1] + resp[2]);

			if ((uint8_t)resp[3] != cksum){
				printf("The checksum failed\n");
				continue;
			}
			*response = *resp;
		}
	}
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_write(void* dev, void* value, int len){
	return UPM_ERROR_NOT_IMPLEMENTED;
}

upm_result_t upm_urm37_read(void* dev, void* value, int* len){
	upm_urm37 device = (upm_urm37) dev;
	int* int_data = value;
	mraa_gpio_write(device->gpio_trig_pin, 0);
	int val = mraa_aio_read(device->aio);
	mraa_gpio_write(device->gpio_trig_pin, 1);
	*int_data = val;
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_get_distance(void* dev, float* distance, upm_distance_u dist_unit){
	upm_urm37 device = (upm_urm37) dev;
	if(device->mode == UPM_ANALOG){
		int val, len;
		// analog mode
		upm_urm37_read(device, &val, &len);
		float volts = ((float)val * (A_REF / device->a_res)) * 1000.0;
		*distance = volts/6.8;
	}
	if(device->mode == UPM_UART){
		// UART mode
		// query distance cmd sequence
		int degrees = device->degrees;
		uint8_t deg = (uint8_t)(degrees / 6);
		if(deg > 46){
			printf("Degrees out of range, must be between 0-270\n");
			return UPM_ERROR_OUT_OF_RANGE;
		}

		// not sure about this string related part
		uint8_t cksum = 0x22 + deg + 0x00;
		char cmd[UPM_URM37_MAX_CMD_LEN];
		cmd[0] = 0x22;
		cmd[1] = deg;
		cmd[2] = 0x00;
		cmd[3] = cksum;

		char response[UPM_URM37_MAX_RESP_LEN];
		upm_urm37_send_command(device, cmd, response, UPM_URM37_MAX_CMD_LEN);
		if(response[1] == '\0' || response[2] == '\0'){
			printf("Send Command failed\n");
			return UPM_ERROR_OPERATION_FAILED;
		}

		uint8_t h = (uint8_t) response[1];
		uint8_t l = (uint8_t) response[2];

		*distance = (float)((h << 8) | l);
	}
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_get_temperature(void* dev, float* temperature, upm_temperature_u temp_unit){
	upm_urm37 device = (upm_urm37) dev;
	if(device->mode == UPM_ANALOG){
		printf("Temperature measurement not available in analog mode\n");
		return UPM_ERROR_NOT_SUPPORTED;
	}
	if(device->mode == UPM_UART){
		char cmd[UPM_URM37_MAX_CMD_LEN];
		cmd[0] = 0x11;
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x11;

		char response[UPM_URM37_MAX_RESP_LEN];
		upm_urm37_send_command(device, cmd, response, UPM_URM37_MAX_CMD_LEN);

		if(response[1] == '\0' || response[2] == '\0'){
			printf("Send Command failed\n");
			return UPM_ERROR_OPERATION_FAILED;
		}

		uint8_t h = (uint8_t) response[1];
		uint8_t l = (uint8_t) response[2];

		*temperature = (float)((h & 0x0f) * 256 + l) / 10.0;
		if (h & 0xf0)
			*temperature *= -1;
	}
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_read_EEPROM(void* dev, uint8_t addr, uint8_t* value){
	upm_urm37 device = (upm_urm37) dev;
	if(device->mode == UPM_ANALOG){
		printf("EEPROM not available in analog mode\n");
		return UPM_ERROR_NOT_SUPPORTED;
	}

	if(addr > 0x04){
		printf("Address must be between 0x00-0x04");
		return UPM_ERROR_OUT_OF_RANGE;
	}

	uint8_t cksum = 0x33 + addr + 0x00;
	char cmd[UPM_URM37_MAX_CMD_LEN];
	cmd[0] = 0x33;
	cmd[1] = addr;
	cmd[2] = 0x00;
	cmd[3] = cksum;

	char response[UPM_URM37_MAX_RESP_LEN];
	upm_urm37_send_command(device, cmd, response, UPM_URM37_MAX_CMD_LEN);

	if(response[1] == '\0' || response[2] == '\0'){
		printf("Send Command failed\n");
		return UPM_ERROR_OPERATION_FAILED;
	}

	*value = response[2];
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_write_EEPROM(void* dev, uint8_t addr, uint8_t value){
	upm_urm37 device = (upm_urm37) dev;
	if(device->mode == UPM_ANALOG){
		printf("EEPROM not available in analog mode\n");
		return UPM_ERROR_NOT_SUPPORTED;
	}

	if(addr > 0x04){
		printf("Address must be between 0x00-0x04");
		return UPM_ERROR_OUT_OF_RANGE;
	}

	uint8_t cksum = 0x44 + addr + value;
	char cmd[UPM_URM37_MAX_CMD_LEN];
	cmd[0] = 0x44;
	cmd[1] = addr;
	cmd[2] = value;
	cmd[3] = cksum;

	char response[UPM_URM37_MAX_RESP_LEN];
	upm_urm37_send_command(device, cmd, response, UPM_URM37_MAX_CMD_LEN);

	if(response[1] == '\0' || response[2] == '\0'){
		printf("Send Command failed\n");
		return UPM_ERROR_OPERATION_FAILED;
	}

	return UPM_SUCCESS;
}

upm_result_t upm_urm37_get_degrees(void* dev, int* degrees){
	upm_urm37 device = (upm_urm37) dev;
	*degrees = device->degrees;
	return UPM_SUCCESS;
}

upm_result_t upm_urm37_set_degrees(void* dev, int degrees){
	upm_urm37 device = (upm_urm37) dev;
	device->degrees = degrees;
	return UPM_SUCCESS;
}