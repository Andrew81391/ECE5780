#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32l476xx.h"

QueueHandle_t xQueueLED = NULL;
QueueHandle_t xQueueIDX = NULL;

int main (void) {
	//set clock to 16MHz
	RCC->CR |= RCC_CR_HSION;
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	SystemCoreClockUpdate();
	
	//gpio setup
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN);
	GPIOA->MODER &= 0xFFFFF3FF;
	GPIOA->MODER |= 0x00000400;
	GPIOC->MODER &= 0xF3FFFFFF;
	
	//gpio pin pa4 analog dac output
	GPIOA->MODER |= GPIO_MODER_MODE4;
	
	//run dac and timer setup
	DACSetup();
	TIM4Setup();
	
	//create queues
	xQueueLED = xQueueCreate(1, sizeof(int32_t));
	xQueueIDX = xQueueCreate(1, sizeof(int32_t));

	if ((xQueueLED != NULL)/* && (xQueueIDX != NULL)*/) {
		//register tasks
		xTaskCreate(
			LEDTask,
			"LEDTask",
			32,
			NULL,
			1,
			NULL
		);
		
		xTaskCreate(
			BTNTask,
			"BTNTask",
			32,
			NULL,
			1,
			NULL
		);
		
		//initialize queues
		int IDXinit = 0;
		xQueueSendToBack(xQueueIDX, &IDXinit, pdMS_TO_TICKS(1));
		int LEDinit = 1;
		xQueueSendToBack(xQueueLED, &LEDinit, pdMS_TO_TICKS(1));
		
		//start scheduler
		vTaskStartScheduler();
	}
	
	for( ;; );
	return 0;
}
