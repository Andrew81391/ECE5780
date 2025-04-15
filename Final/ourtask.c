#include "ourtask.h"
#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>
#define TIMEOUT_DURATION 50000


extern QueueHandle_t xQueueLED;
extern QueueHandle_t xQueueUART;
extern QueueHandle_t xQueueSENSOR;
extern TaskHandle_t LEDTaskHandle;
extern TaskHandle_t BTNTaskHandle;
extern TaskHandle_t sensorTaskHandle;
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
		char timeout_flag = 0;
		int32_t timeout = 0;
		BaseType_t status = xQueueReceive(xQueueSENSOR, &com, 0);
		char temp;
		char tempString[15]="balls";
		int prox;
		char proxString[15]="NOOO";
		if (status == pdPASS) {
			if (com == 't') {
				timeout=0;
				while (!(USART3->ISR & USART_ISR_TXE));
				USART3->TDR = 0x50;
				//for (volatile int i =0; i<500; i++);
				while (!(USART3->ISR & USART_ISR_RXNE)){
					timeout++;
					if (timeout==TIMEOUT_DURATION){
						timeout_flag=1;
						break;
					}
				}
				if (timeout_flag==0) {
					timeout=0;
					temp = ((char)USART3->RDR)-45;
					temp = (char)((9.0/5.0) * temp + 27);
					
					//sprintf(tempString, "%d F\r\n", temp);
					for (int i=0; tempString[i] != NULL; i++) {
						while ((USART2->ISR & USART_ISR_TXE)!=USART_ISR_TXE){
							timeout++;
							if (timeout==TIMEOUT_DURATION){
								timeout_flag=1;
								break;
							}
						}
						if (timeout_flag == 0){
							USART2->TDR = tempString[i];
						}
					}
				}
			}
			else if (com=='p') {
				
				uint16_t proxRaw;
				if (UART3_Read2Bytes(&proxRaw, TIMEOUT_DURATION, 0x55) == 0) {
						int proxInches = proxRaw / 25.4;

						const char msg[] = "Distance: ";
						for (int i = 0; msg[i] != '\0'; i++) {
								while (!(USART2->ISR & USART_ISR_TXE));
								USART2->TDR = msg[i];
						}

						// Convert and print value (you can improve this later)
						char buf[10];
						int len = sprintf(buf, "%d\r\n", proxInches);
						for (int i = 0; i < len; i++) {
								while (!(USART2->ISR & USART_ISR_TXE));
								USART2->TDR = buf[i];
						}
				} else {
						const char errorMsg[] = "UART read error\r\n";
						for (int i = 0; errorMsg[i] != '\0'; i++) {
								while (!(USART2->ISR & USART_ISR_TXE));
								USART2->TDR = errorMsg[i];
						}
				}
				
				
				
//				timeout=0;
//				while (!(USART3->ISR & USART_ISR_TXE));
//				USART3->TDR = 0x55;
//				//for (volatile int i =0; i<500; i++);
//				while (!(USART3->ISR & USART_ISR_RXNE)){
//					timeout++;
//					if (timeout==TIMEOUT_DURATION){
//						timeout_flag=1;
//						break;
//					}
//				}
//				if (timeout_flag==0){
//					timeout=0;
//					prox = (USART3->RDR) << 8;
//					while (!(USART3->ISR & USART_ISR_RXNE)){
//						timeout++;
//						if (timeout == TIMEOUT_DURATION){
//							timeout_flag=1;
//							break;
//						}
//						
//					}
//					if (timeout_flag==0){
//						timeout=0;
//						prox |= USART3->RDR;
//						//prox = (int)(prox/25.4);
//						//sprintf(proxString, "%d mm\r\n", prox);
//						taskENTER_CRITICAL();
//						for (int i=0; proxString[i] != NULL; i++) {
//							while ((USART2->ISR & USART_ISR_TXE)!=USART_ISR_TXE){
//								timeout++;
//								if (timeout == TIMEOUT_DURATION) {
//									timeout_flag = 1;
//									break;
//								}
//							}
//							if (timeout_flag == 0){
//								USART2->TDR = proxString[i];
//							}
//						}
//						taskEXIT_CRITICAL();
//					}
//				}
			}
		}
	}
}

void TIM4_IRQHandler() {
	//eTaskState sensorTaskState = 0x10;
	UBaseType_t LEDStackLeft;
	UBaseType_t BTNStackLeft;
	UBaseType_t sensorStackLeft;
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
				case 's':
					//sensorTaskState = eTaskGetState(sensorTaskHandle);
					LEDStackLeft = uxTaskGetStackHighWaterMark(LEDTaskHandle);
					BTNStackLeft = uxTaskGetStackHighWaterMark(BTNTaskHandle);
					sensorStackLeft = uxTaskGetStackHighWaterMark(sensorTaskHandle);
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

int UART3_Read2Bytes(uint16_t *outVal, int32_t timeoutLimit, char command) {
    int32_t timeout = 0;

    // Clear any previous errors
    if (USART3->ISR & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE)) {
        volatile char flush = USART3->RDR;
        USART3->ICR |= (USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF);
    }
		// --- Send Command ---
		while (!(USART3->ISR & USART_ISR_TXE));
				USART3->TDR = command;
    // --- First Byte ---
    timeout = 0;
    while (!(USART3->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit) return -1;
    }
    uint8_t msb = USART3->RDR;

    // --- Second Byte ---
    timeout = 0;
    while (!(USART3->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit) return -2;
    }
    uint8_t lsb = USART3->RDR;

    *outVal = ((uint16_t)msb << 8) | lsb;
    return 0;
}