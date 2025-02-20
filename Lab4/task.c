#include "task.h"
#include "queue.h"
#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include <stdio.h>


extern QueueHandle_t xQueueLED;
extern QueueHandle_t xQueueUART;
extern QueueHandle_t xQueueSENSOR;
extern const uint16_t sinLUT[];

void LEDTask (void *pvParameters) {
	for ( ;; ) {
		int32_t LEDState;
		BaseType_t status = xQueuePeek(xQueueLED, &LEDState, 0);
		if (status == pdPASS) {
			if (LEDState == 1) {
				GPIOA->ODR |= 0x00000020;
			}
			else {
				GPIOA->ODR &= 0xFFFFFFDF;
			}
		}
	}
}

void BTNTask (void *pvParameters) {
	for ( ;; ) {
		int32_t LEDState;
		BaseType_t status = xQueuePeek(xQueueLED, &LEDState, 0);
		if (status == pdPASS) {
			if ((GPIOC->IDR & 0x00002000 ) == 0) {
				for (volatile int i = 0; i < 25000; i++);
				if ((GPIOC->IDR & 0x00002000) == 0) {
					if (LEDState) {
						LEDState = 0;
						xQueueOverwrite(xQueueLED, &LEDState);
					}
					else {
						LEDState = 1;
						xQueueOverwrite(xQueueLED, &LEDState);
					}
				}
			}
		}
	}
}

void SENSORTask (void *pvParameters) {
	for ( ;; ) {
		char com;
		BaseType_t status = xQueueReceive(xQueueSENSOR, &com, portMAX_DELAY);
		if (status == pdPASS) {
			if (com == 't') {
				while (!(USART3->ISR & USART_ISR_TXE));
				USART3->TDR = 0x50;
				for (volatile int i =0; i<500; i++);
				while (!(USART3->ISR & USART_ISR_RXNE));
				char temp = ((char)USART3->RDR)-45;
				temp = (9.0/5.0) * temp + 27;
				char tempString[15];
				sprintf(tempString, "%d deg F\r\n", temp);
				for (int i=0; tempString[i] != NULL; i++) {
					while ((USART2->ISR & USART_ISR_TXE)!=USART_ISR_TXE);
					USART2->TDR = tempString[i];
				}
			}
			else {
				while (!(USART3->ISR & USART_ISR_TXE));
				USART3->TDR = 0x55;
				for (volatile int i =0; i<500; i++);
				int prox;
				while (!(USART3->ISR & USART_ISR_RXNE));
				prox = (USART3->RDR) << 8;
				while (!(USART3->ISR & USART_ISR_RXNE));
				prox |= USART3->RDR;
				prox = (int)(prox/25.4);
				char proxString[15];
				sprintf(proxString, "%d inches\r\n", prox);
				for (int i=0; proxString[i] != NULL; i++) {
					while ((USART2->ISR & USART_ISR_TXE)!=USART_ISR_TXE);
					USART2->TDR = proxString[i];
				}
			}
		}
	}
}

void TIM4_IRQHandler() {
	static int32_t index = 0;
	if ((TIM4->SR & TIM_SR_CC1IF) != 0) {	
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		//get note from uart queue
		if (uxQueueMessagesWaitingFromISR(xQueueUART) != 0) {
			char note;
			xQueueReceiveFromISR(xQueueUART, &note, 0);
			switch (note) {
				case 'a':
					TIM4->ARR = 141;
					break;
				case 'b':
					TIM4->ARR = 126;
					break;
				case 'c':
					TIM4->ARR = 118;
					break;
				case 'd':
					TIM4->ARR = 105;
					break;
				case 'e':
					TIM4->ARR = 94;
					break;
				case 'f':
					TIM4->ARR = 88;
					break;
				case 'g':
					TIM4->ARR = 79;
					break;
				case 'h':
					TIM4->ARR = 70;
					break;
				case 't': {
					xQueueSendToBackFromISR(xQueueSENSOR, &note, &xHigherPriorityTaskWoken);
					portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					break;
				}
				case 'p': {
					xQueueSendToBackFromISR(xQueueSENSOR, &note, &xHigherPriorityTaskWoken);
					portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					break;
				}
			}	
		}
		
		
		int32_t LEDState;
		BaseType_t status1 = xQueuePeekFromISR(xQueueLED, &LEDState);
		
		if (status1 == pdPASS){
			//set DAC output according to the lookup table
			DAC->DHR12R1 &= 0xFFFFF000;
			DAC->DHR12R1 |= sinLUT[index];
			DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
			
			//increment the lookup table index
			if (LEDState) {
				index += 1;
			}
			if (index >= 64) {
				index = 0;
			}
		}
		
		TIM4->SR &= ~TIM_SR_CC1IF;
	}
	
	if ((TIM4->SR & TIM_SR_UIF) != 0)
		TIM4->SR &= ~TIM_SR_UIF;
	
	return;
}

void DACSetup() {
	//enable clock to DAC
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
	
	//disable DAC
	DAC->CR &= ~(DAC_CR_EN1 | DAC_CR_EN2);
	
	//DAC mode control to buffered external
	DAC->MCR &= ~DAC_MCR_MODE1;
	
	//enable trigger for DAC channel 1
	DAC->CR |= DAC_CR_TEN1;
	
	//clear trigger selection for channel 1
	DAC->CR |= DAC_CR_TSEL1;
	
	//select timer 4 as trigger for DAC channel 1
	//DAC->CR |= (DAC_CR_TSEL1_0 | DAC_CR_TSEL1_2);
	
	//enable DAC channel 1
	DAC->CR |= DAC_CR_EN1;
}

void TIM4Setup() {
	//enable peripheral timer
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	//disable the counter
	TIM4->CR1 &= ~TIM_CR1_CEN;
	
	//enable buffer
	TIM4->CR1 |= TIM_CR1_ARPE;
	
	//clear status register and counter
	TIM4->SR = 0;
	TIM4->CNT = 0;
	
	//set edge align mode
	TIM4->CR1 &= ~TIM_CR1_CMS;
	
	//select up counting
	TIM4->CR1 &= ~TIM_CR1_DIR;
	
	//set update event as trigger output
	TIM4->CR2 &= ~TIM_CR2_MMS;
	TIM4->CR2 |= TIM_CR2_MMS_1;
		
	//enable update and trigger interrupts
	TIM4->DIER |= TIM_DIER_TIE;
	TIM4->DIER |= TIM_DIER_UIE;
	
	//set division rate for 440Hz
	TIM4->PSC = 3;
	TIM4->ARR = 141;
	
	//set duty cycle
	TIM4->CCR1 = 35;
	TIM4->CCER |= TIM_CCER_CC1E;
	
	//
	TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM4->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2);
		
	//enable the counter
	TIM4->CR1 |= TIM_CR1_CEN;
	
	//enable timer 4 interrupt
	NVIC_EnableIRQ(TIM4_IRQn);
}
