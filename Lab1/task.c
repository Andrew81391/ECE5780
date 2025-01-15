#include "task.h"
#include "stm32l476xx.h"

extern volatile int LEDState;

void LEDTask (void *pvParameters) {
	for ( ;; ) {
		if (LEDState == 1) {
			GPIOA->ODR |= 0x00000020;
		}
		else {
			GPIOA->ODR &= 0xFFFFFFDF;
		}
	}
}

void BTNTask (void *pvParameters) {
	for ( ;; ) {
		if ((GPIOC->IDR & 0x00002000 ) == 0) {
			for (volatile int i = 0; i < 15000; i++);
			if ((GPIOC->IDR & 0x00002000) == 0) {
				if (LEDState)
					LEDState = 0;
				else
					LEDState = 1;
			}
		}
	}
}