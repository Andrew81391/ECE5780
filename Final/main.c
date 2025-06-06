#include "FreeRTOS.h"
#include "ourtask.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32l476xx.h"
#include "UART.h"

QueueHandle_t xQueueLED = NULL;
QueueHandle_t xQueueVOL = NULL;
TaskHandle_t LEDTaskHandle = NULL;
TaskHandle_t BTNTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t FREQTaskHandle = NULL;
SemaphoreHandle_t uartMutex = NULL;
StaticSemaphore_t uartMutexBuffer;

const uint16_t sinLUT[] = {
		0x05DC,0x063E,0x069F,0x06FE,0x075B,0x07B3,0x0808,
		0x0856,0x089F,0x08E1,0x091B,0x094E,0x0978,0x0999,
		0x09B1,0x09BF,0x09C4,0x09BF,0x09B1,0x0999,0x0978,
		0x094E,0x091B,0x08E1,0x089F,0x0856,0x0808,0x07B3,
		0x075B,0x06FE,0x069F,0x063E,0x05DC,0x057A,0x0519,
		0x04BA,0x045D,0x0405,0x03B0,0x0362,0x0319,0x02D7,
		0x029D,0x026A,0x0240,0x021F,0x0207,0x01F9,0x01F4,
		0x01F9,0x0207,0x021F,0x0240,0x026A,0x029D,0x02D7,
		0x0319,0x0362,0x03B0,0x0405,0x045D,0x04BA,0x0519,
		0x057A 
};

int main (void) {
	//set clock to 16MHz
	RCC->CR |= RCC_CR_HSION;
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	SystemCoreClockUpdate();
	
	//gpio setup
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOBEN);
	GPIOA->MODER &= 0xFFFFF3FF;
	GPIOA->MODER |= 0x00000400;
	GPIOC->MODER &= 0xF3FFFFFF;
	
	//gpio pin pa4 analog dac output
	GPIOA->MODER |= GPIO_MODER_MODE4;
	
	//run dac and timer setup
	DACSetup();
	TIM4Setup();
	//USART_Init_US100();
	USART_Init_PC();
  
	//create queues
	xQueueLED = xQueueCreate(1, sizeof(int32_t));
	xQueueVOL = xQueueCreate(1, sizeof(uint16_t));
	
	uartMutex = xSemaphoreCreateMutexStatic(&uartMutexBuffer);

	if ((xQueueLED != NULL) && (xQueueVOL != NULL) && (uartMutex != NULL)) {
		//register tasks
		xTaskCreate(
			LEDTask,
			"LEDTask",
			32,
			NULL,
			1,
			&LEDTaskHandle
		);
		
		xTaskCreate(
			BTNTask,
			"BTNTask",
			32,
			NULL,
			1,
			&BTNTaskHandle
		);
		
		xTaskCreate(
			FREQTask,
			"FREQTask",
			256,
			NULL,
			1,
			&FREQTaskHandle
		);
		
		xTaskCreate(
			VOLTask,
			"VOLTask",
			256,
			NULL,
			1,
			NULL
		);
		
		
		//initialize queues
		int init = 0;
		xQueueSendToBack(xQueueLED, &init, pdMS_TO_TICKS(1));
		
		xQueueSendToBack(xQueueVOL, &init, pdMS_TO_TICKS(1));
		
		//start scheduler
		vTaskStartScheduler();
	}
	
	for( ;; );
	return 0;
}
