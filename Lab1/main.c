#include "FreeRTOS.h"
#include "task.h"
#include "stm32l476xx.h"

volatile int LEDState=0;

int main (void) {
	//gpio setup
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOCEN;
	GPIOA->MODER &= 0xFFFFF3FF;
	GPIOA->MODER |= 0x00000400;
	GPIOC->MODER &= 0xF3FFFFFF;

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
	
	vTaskStartScheduler();
	
	for( ;; );
	return 0;
}
