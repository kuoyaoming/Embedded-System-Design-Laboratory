#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "system_mt7687.h"
#include "top.h"

#ifdef _GNUC_
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}

static void SystemClock_Config(void)
{
    top_xtal_init();
}

static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
		hal_gpio_init(HAL_GPIO_36);
		hal_gpio_init(HAL_GPIO_37);
		hal_pinmux_set_function(HAL_GPIO_36, 8);
		hal_pinmux_set_function(HAL_GPIO_37, 8);
		hal_gpio_set_direction(HAL_GPIO_36, HAL_GPIO_DIRECTION_OUTPUT);
		hal_gpio_set_direction(HAL_GPIO_37, HAL_GPIO_DIRECTION_INPUT);
}

int main(void)
{
    SystemClock_Config();
    prvSetupHardware();
    __enable_irq();
    __enable_fault_irq();
		int t = 0;
		int ledt = 0;
		_Bool st = 0;
		printf("START!!!\r\n");
		printf("LED FLASH = 0\r\n");
		while (1) {
			
			hal_gpio_data_t button;
			hal_gpio_get_input(HAL_GPIO_37, &button);
			
			if(button){
				t = t + 1;
			}else{
				if(t > 15){
					st = !st;
					if(st){
						printf("LED FLASH = 1\r\n");
					}else{
						printf("LED FLASH = 0\r\n");
					}
				}
				t = 0;
			}
			
			if(st){
				ledt = ledt + 1;
				if(ledt<500){
					hal_gpio_set_output(HAL_GPIO_36, HAL_GPIO_DATA_HIGH);
				}else if(ledt<1000){
					hal_gpio_set_output(HAL_GPIO_36, HAL_GPIO_DATA_LOW);
				}else{
					ledt = 0;
				}
			}else{
				hal_gpio_set_output(HAL_GPIO_36, HAL_GPIO_DATA_LOW);
				ledt = 0;
			}
			
			hal_gpt_delay_ms(1);
			//printf("t=%d ledt=%d st=%d\r\n",t,ledt,st);
		}
}
