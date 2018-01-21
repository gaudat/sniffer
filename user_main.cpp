/*
 * sniffer_main.c
 *
 *  Created on: 31 Dec 2017
 *      Author: m2
 */


#include "esp_common.h"
#include "uart.h"
#include "gpio.h"
#include "globals.h"
#include "sniffer_backend.h"
#include "serial_handler.h"

void blinker(void* arg) {
    // Set LED GPIO pin to output
    GPIO_AS_OUTPUT(LED_GPIO);
    while(1) {
    	GPIO_OUTPUT(LED_GPIO, LED_OFF);
    	vTaskDelay(1/portTICK_RATE_MS);
    }
	while(1) {
	GPIO_OUTPUT(LED_GPIO, LED_ON);
	vTaskDelay(200/portTICK_RATE_MS);
	GPIO_OUTPUT(LED_GPIO, LED_OFF);
	vTaskDelay(200/portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}

void flash_key_handler(void* arg) {
	while (1) {
	bool flash_pressed = !GPIO_INPUT_GET(GPIO_ID_PIN(0));
	static bool flash_pressed_prev = false;
	static bool triggered = false;

	if (flash_pressed && !flash_pressed_prev && !triggered) {
		vTaskDelay(50/portTICK_RATE_MS); // Debounce
		if (flash_pressed) {
			// DO WORK HERE
			printf("flash_key: pressed\n");
			triggered = true;
		}
	}
	if (!flash_pressed) triggered = false;

	flash_pressed_prev = flash_pressed;
	}
	vTaskDelete(NULL);
}

/**
 * A function left over from the sample provided by Espressif.
 *
 * @return The location of RF calibration sectors in the flash.
 */
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

/**
 * One function to start it all.
 */
void user_init(void) {
	UART_SetBaudrate(0, 921600);

	initialize_globals();

    printf("SDK version:%s\n", system_get_sdk_version());
    printf("Hello world!\n");
    printf("Phy mode: %d\n", wifi_get_phy_mode());

    xTaskCreate(blinker, "blinker", 256, NULL, 3, NULL);
    xTaskCreate(flash_key_handler, "flash_key_handle", 256, NULL, 3, NULL);
    xTaskCreate(serial_handler, "serial_handler", 256, NULL, 3, NULL);
    xTaskCreate(sniffer_backend, "sniffer_backend", 512, NULL, 3, NULL);

    // Interrupt as soon as the UART receives one byte
    UART_IntrConfTypeDef uart_intr;

    // RX FIFO full interrupt mask is bit 0
    uart_intr.UART_IntrEnMask = UART_RXFIFO_FULL_INT_ENA;
    uart_intr.UART_RX_FifoFullIntrThresh = 1;
    UART_IntrConfig(0, &uart_intr);
    UART_intr_handler_register(serial_intr_handler, NULL);
    ETS_UART_INTR_ENABLE();
}
