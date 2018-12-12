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
	hal_gpio_init(HAL_GPIO_0);
	hal_gpio_init(HAL_GPIO_39);
	hal_gpio_init(HAL_GPIO_37);
	hal_pinmux_set_function(HAL_GPIO_0, 8);
	hal_pinmux_set_function(HAL_GPIO_39, 8);
	hal_pinmux_set_function(HAL_GPIO_37, 8);
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_OUTPUT);		//i2c_data
	hal_gpio_set_direction(HAL_GPIO_39, HAL_GPIO_DIRECTION_OUTPUT);		//i2c_clk
	hal_gpio_set_direction(HAL_GPIO_37, HAL_GPIO_DIRECTION_INPUT);
}
//==================================================================================================

uint16_t data[13] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static uint16_t ledid[11] = {0,1,3,7,15,31,63,127,255,511,1023};
int t = 0;
unsigned char ledst = 0;

int main(void)
{
    SystemClock_Config();
    prvSetupHardware();
    __enable_irq();
    __enable_fault_irq();
	hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
	hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_LOW);
	hal_gpt_delay_ms(1);
	
	while (1) {
		
		hal_gpio_data_t button;
		hal_gpio_get_input(HAL_GPIO_37, &button);
		
		if(button){
			t = t + 1;
		}else{
			if(t > 15){
				ledst++;
				if(ledst>10){
					ledst = 1;
					for(int i=0;i<13;i++)data[i]=0x0000;
				}
				data[12-ledst]=0x00ff;
				printf("LED BAR = %i \r\n",ledid[ledst]);
				/*
				for(int i=12 ; i >-1 ; i--){
					for(int j=15 ; j > -1; j--){
						printf("%d",(data[i]>>j)&1);//
					}
					printf("\r\n");
				}
				*/
			}
			t = 0;
		}
		_Bool clk = 0;
		hal_gpt_delay_us(1);
		for(int i=12 ; i >-1 ; i--){
			for(int j=15 ; j > -1; j--){
				if((data[i]>>j)&1){
					hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_HIGH);
				}else{
					hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
				}
				hal_gpt_delay_us(1);
				clk = !clk;
				if(clk){
					hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
				}else{
					hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_LOW);
				}
				hal_gpt_delay_us(1);
				
			}
		}
		hal_gpt_delay_us(230);
		for(int i=0 ; i<8 ; i++){
			clk = !clk;
			if(clk){
				hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_HIGH);
			}else{
				hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
			}
			hal_gpt_delay_us(1);
		}
		clk = 0;
	}
}