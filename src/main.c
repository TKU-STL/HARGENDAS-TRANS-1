/*
Author: Mark Chen, STL Avionics Engineer
Date: 2022-July-7th
Reference: Zephyr RTOS UART Driver Sample Code

Changes since last work session:
*/

#include <kernel.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>
#include <string.h>

//#define USART3 = 1

//#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

//const struct device *sl_uart1 = DEVICE_DT_GET(UART_DEVICE_NODE);
const struct device *sl_uart1 = DEVICE_DT_GET(DT_NODELABEL(usart3));

// Configure the UART device to set the baud rate to 921600 baud
struct uart_config uart_cfg = {
	.baudrate = 921600,
	UART_CFG_PARITY_NONE,
	UART_CFG_STOP_BITS_1,
	UART_CFG_DATA_BITS_8,
	UART_CFG_FLOW_CTRL_NONE,
};

void uart_write(const char *str)
{
	uint8_t i;
	uint8_t temp;
	// unsigned char recvChar;
	for (i = 0; i < strlen(str); i++)
	{
		temp = str[i];
		// printk("%c", temp);
		uart_poll_out(sl_uart1, temp);
	}
}

#define MSG_SIZE 1024
/*
void uart_read(){
	//uint8_t *message[MSG_SIZE];
	uint8_t *message;
	if (uart_poll_in(sl_uart1, message) == 0)
	{
		// printk("%c", *message);
		//uart_write(message);
		for (int i = 0; i < MSG_SIZE; i++)
		{
			printk("%c", message[i]);
		}
		//printk("%s", message);
	}else {
		printk("No data received\n");
	}
}
*/
#define MSG_SIZE 1024

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

/* receive buffer used in UART ISR callback */
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;

void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(sl_uart1)) {
		return;
	}

	while (uart_irq_rx_ready(sl_uart1)) {

		uart_fifo_read(sl_uart1, &c, 1);

		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(sl_uart1, buf[i]);
	}
}

//Convert a unsigned char into a string
char *itoa(int value, char *str, int base)
{
	int i = 0;
	int isNegative = 0;
	if (value == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
	if (value < 0 && base == 10) {
		isNegative = 1;
		value = -value;
	}
	while (value != 0) {
		int rem = value % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		value = value / base;
	}
	if (isNegative) {
		str[i++] = '-';
	}
	str[i] = '\0';
	reverse(str, i);
	return str;
}


void main(void)
{
	if (!device_is_ready(sl_uart1))
	{
		printk("uart devices not ready\n");
		return;
	}

	// Configure the UART device
	int status = uart_configure(sl_uart1, &uart_cfg);
	if (status != 0)
	{
		printk("uart_configure failed\n");
		return;
	}
	else
	{
		printk("uart_configure success\n");
	}

	// Create a buffer to store the data received from the UART device
	char *buf = "AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000Acc0";
	uint8_t *buf2 = "AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000 AccX: 10000.0000 AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000AccX: 10000.0000Acc0";
	// Write a string to the UART device with buffer
	uint8_t *buf3 = "Hello World!\n";
	unsigned char Buff;
	//uint8_t *command; 
	//uint8_t message;
	/*
	while (true)
	{
		// Send a string to the UART device
		//uart_write(buf2);
		uart_poll_in(sl_uart1, Buff);
	
		//printk("%d\n", strlen(*Buff));
		printk("%c\n", *Buff);
		//printk("%s\n", command);
		//uart_async_write(buf2);
		// Print the string read from the UART device
		// printk("%s\n", buf);

		// Delay for 500 Milliseconds
		k_sleep(K_MSEC(1000));
	}*/
	//Create a buffer to recieve the data from the UART device
	unsigned char buf4;

	//print_uart("Hello! I'm your echo bot.\r\n");
	//print_uart("Tell me something and press enter:\r\n");

	//Continously recieve the data from the UART device and print it to the console from HEX to ASCII

	while (true) {
		// Wait for data to be received
		k_sleep(K_MSEC(1000));
		// Read the data from the UART device
		uart_poll_in(sl_uart1, &buf4);
		// Print the data received from the UART device
		//itoa(buf4, buf, 16);
		printk("%hhx", buf4);
		// Send the data received from the UART device back to the UART device
		//uart_poll_out(sl_uart1, *Buff);
	}
}