#include "ourtask.h"
#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#define TIMEOUT_DURATION 5000


extern QueueHandle_t xQueueLED;
extern QueueHandle_t xQueueUART;
extern QueueHandle_t xQueueVOL;
extern TaskHandle_t LEDTaskHandle;
extern TaskHandle_t BTNTaskHandle;
extern TaskHandle_t sensorTaskHandle;
extern TaskHandle_t FREQTaskHandle;
extern SemaphoreHandle_t uartMutex;
extern StaticSemaphore_t uartMutexBuffer;
extern const uint16_t sinLUT[];
static uint16_t global_scalar;

static const char* note_names[] = {
    "C3\r\n", "C#3\r\n", "D3\r\n", "D#3\r\n", "E3\r\n", "F3\r\n", "F#3\r\n", "G3\r\n", "G#3\r\n", "A3\r\n", "A#3\r\n", "B3\r\n",
    "C4\r\n", "C#4\r\n", "D4\r\n", "D#4\r\n", "E4\r\n", "F4\r\n", "F#4\r\n", "G4\r\n", "G#4\r\n", "A4\r\n", "A#4\r\n", "B4\r\n",
    "C5\r\n"
};

static const char* vol_names[] = {
	"Vol 0\r\n", "Vol 1\r\n", "Vol 2\r\n", "Vol 3\r\n", "Vol 4\r\n", "Vol 5\r\n", "Vol 6\r\n", "Vol 7\r\n"
};

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

void FREQTask (void *pvParameters) {
	const TickType_t xDelay = 40/portTICK_PERIOD_MS;
	Note lastNote = NULL;
	Note currNote = NULL;
	for ( ;; ) {
		uint16_t prox;
		
		xSemaphoreTake(uartMutex, portMAX_DELAY);
		int read_status = UART3_Read2Bytes(&prox, TIMEOUT_DURATION, 0x55);
		xSemaphoreGive(uartMutex);
		
		if (read_status == 0) {

			if (prox > 0 && prox < 75) {
				TIM4->ARR = 239;
				currNote = C3;
			} else if (prox >= 75 && prox < 100) {
				TIM4->ARR = 225;
				currNote = Cs3;
			} else if (prox >= 100 && prox < 125) {
				TIM4->ARR = 213;
				currNote = D3;
			} else if (prox >= 125 && prox < 150) {
				TIM4->ARR = 201;
				currNote = Ds3;
			} else if (prox >= 150 && prox < 175) {
				TIM4->ARR = 190;
				currNote = E3;
			} else if (prox >= 175 && prox < 200) {
				TIM4->ARR = 179;
				currNote = F3;
			} else if (prox >= 200 && prox < 225) {
				TIM4->ARR = 169;
				currNote = Fs3;
			} else if (prox >= 225 && prox < 250) {
				TIM4->ARR = 159;
				currNote = G3;
			} else if (prox >= 250 && prox < 275) {
				TIM4->ARR = 150;
				currNote = Gs3;
			} else if (prox >= 275 && prox < 300) {
				TIM4->ARR = 142;
				currNote = A3;
			} else if (prox >= 300 && prox < 325) {
				TIM4->ARR = 134;
				currNote = As3;
			} else if (prox >= 325 && prox < 350) {
				TIM4->ARR = 126;
				currNote = B3;
			} else if (prox >= 350 && prox < 375) {
				TIM4->ARR = 119;
				currNote = C3;
			} else if (prox >= 375 && prox < 400) {
				TIM4->ARR = 113;
				currNote = Cs4;
			} else if (prox >= 400 && prox < 425) {
				TIM4->ARR = 106;
				currNote = D4;
			} else if (prox >= 425 && prox < 450) {
				TIM4->ARR = 100;
				currNote = Ds4;
			} else if (prox >= 450 && prox < 475) {
				TIM4->ARR = 95;
				currNote = E4;
			} else if (prox >= 475 && prox < 500) {
				TIM4->ARR = 89;
				currNote = F4;
			} else if (prox >= 500 && prox < 525) {
				TIM4->ARR = 84;
				currNote = Fs4;
			} else if (prox >= 525 && prox < 550) {
				TIM4->ARR = 80;
				currNote = G4;
			} else if (prox >= 550 && prox < 575) {
				TIM4->ARR = 75;
				currNote = Gs4;
			} else if (prox >= 575 && prox < 600) {
				TIM4->ARR = 71;
				currNote = A4;
			} else if (prox >= 600 && prox < 625) {
				TIM4->ARR = 67;
				currNote = As4;
			} else if (prox >= 625 && prox < 650) {
				TIM4->ARR = 63;
				currNote = B4;
			} else if (prox >= 650) {
				TIM4->ARR = 60;
				currNote = C5;
			}
			
			if (currNote != lastNote) {
				lastNote = currNote;
				
				xSemaphoreTake(uartMutex, portMAX_DELAY);
				
				const char* note = note_names[currNote];
				for (int i = 0; note[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = note[i];
				}
				
				xSemaphoreGive(uartMutex);
			}
		} 
		/*
		else if (read_status == -1) {
				xSemaphoreTake(uartMutex, portMAX_DELAY);

				const char errorMsg[] = "freq read err b1\r\n";
				for (int i = 0; errorMsg[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = errorMsg[i];
				}
				
				xSemaphoreGive(uartMutex);
		} else if (read_status == -2) {
				xSemaphoreTake(uartMutex, portMAX_DELAY);

				const char errorMsg[] = "freq read err b2\r\n";
				for (int i = 0; errorMsg[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = errorMsg[i];
				}
				
				xSemaphoreGive(uartMutex);
		}
		*/
		
		vTaskDelay(xDelay);
		
		//xSemaphoreGive(uartMutex);
	}
}

void VOLTask (void *pvParameters) {
	const TickType_t xDelay = 40/portTICK_PERIOD_MS;
	Volume lastVol = NULL;
	Volume currVol = NULL;
	for ( ;; ) {
		
		uint16_t prox;
		uint16_t scalar = 0;
		
		xSemaphoreTake(uartMutex, portMAX_DELAY);
		int read_status = UART1_Read2Bytes(&prox, TIMEOUT_DURATION, 0x55);
		xSemaphoreGive(uartMutex);
		
		if (read_status == 0) {

			if (prox > 0 && prox < 75) {
				scalar = 0;
				currVol = V0;
			} else if (prox >= 75 && prox < 100) {
				scalar = 1;
				currVol = V1;
			} else if (prox >= 100 && prox < 125) {
				scalar = 2;
				currVol = V2;
			} else if (prox >= 125 && prox < 150) {
				scalar = 3;
				currVol = V3;
			} else if (prox >= 150 && prox < 175) {
				scalar = 4;
				currVol = V4;
			} else if (prox >= 175 && prox < 200) {
				scalar = 5;
				currVol = V5;
			} else if (prox >= 200 && prox < 225) {
				scalar = 6;
				currVol = V6;
			} else if (prox >= 225 && prox < 450) {
				scalar = 7;
				currVol = V7;
			} else if (prox >=  450) {
				scalar = 0;
				currVol = V0;
			} 
			
			if (currVol != lastVol) {
				lastVol = currVol;
				
				//xQueueOverwrite(xQueueVOL, &scalar);
				global_scalar = scalar;
				
				xSemaphoreTake(uartMutex, portMAX_DELAY);
				
				const char* vol = vol_names[currVol];
				for (int i = 0; vol[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = vol[i];
				}
				
				xSemaphoreGive(uartMutex);
			}
		} 
		/*
		else if (read_status == -1) {
				xSemaphoreTake(uartMutex, portMAX_DELAY);
	
				const char errorMsg[] = "vol read err b1\r\n";
				for (int i = 0; errorMsg[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = errorMsg[i];
				}
				
				xSemaphoreGive(uartMutex);
		} else if (read_status == -2) {
				xSemaphoreTake(uartMutex, portMAX_DELAY);
	
				const char errorMsg[] = "vol read err b2\r\n";
				for (int i = 0; errorMsg[i] != '\0'; i++) {
						while (!(USART2->ISR & USART_ISR_TXE));
						USART2->TDR = errorMsg[i];
				}
				
				xSemaphoreGive(uartMutex);
		}
		*/
		
		vTaskDelay(xDelay);
		//xSemaphoreGive(uartMutex);
	}
}

void TIM4_IRQHandler() {
	static int32_t index = 0;
	if ((TIM4->SR & TIM_SR_CC1IF) != 0) {	
		
		//uint16_t scalar;
		//BaseType_t status1 = xQueuePeekFromISR(xQueueVOL, &scalar);
		
		int32_t LEDState;
		BaseType_t status2 = xQueuePeekFromISR(xQueueLED, &LEDState);
		
		if (status2 == pdPASS){
			//set DAC output according to the lookup table
			if (LEDState && global_scalar <= 7) {
				DAC->DHR12R1 &= 0xFFFFF000;
				DAC->DHR12R1 |= (((sinLUT[index])*global_scalar)/7)&0x00000FFF;
				DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
			}
			
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
		taskENTER_CRITICAL();
	
    int32_t timeout = 0;

    // Clear any previous errors
    if (USART3->ISR & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE)) {
        volatile char flush = USART3->RDR;
        USART3->ICR |= (USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF);
    }
		// --- Send Command ---
		while (!(USART3->ISR & USART_ISR_TXE));
				USART3->TDR = command;
		
		while(!(USART3->ISR & USART_ISR_TC));
		//volatile char flush = USART3->RDR;
		
    // --- First Byte ---
    timeout = 0;
    while (!(USART3->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit){
					taskEXIT_CRITICAL();
					return -1;
				}
    }
    uint8_t msb = USART3->RDR;

    // --- Second Byte ---
    timeout = 0;
    while (!(USART3->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit){
					taskEXIT_CRITICAL();
					return -2;
				}
    }
    uint8_t lsb = USART3->RDR;

    *outVal = ((uint16_t)msb << 8) | lsb;
		
		taskEXIT_CRITICAL();
    return 0;
}

int UART1_Read2Bytes(uint16_t *outVal, int32_t timeoutLimit, char command) {
		taskENTER_CRITICAL();
	
    int32_t timeout = 0;
		
    // Clear any previous errors
    if (USART1->ISR & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE)) {
        volatile char flush = USART1->RDR;
        USART1->ICR |= (USART_ICR_ORECF | USART_ICR_FECF | USART_ICR_NCF);
    }
		// --- Send Command ---
		while (!(USART1->ISR & USART_ISR_TXE));
		USART1->TDR = command;
		
		while(!(USART1->ISR & USART_ISR_TC));
		//volatile char flush = USART1->RDR;
		
    // --- First Byte ---
    timeout = 0;
    while (!(USART1->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit){
					taskEXIT_CRITICAL();
					return -1;
				}
    }
    uint8_t msb = USART1->RDR;

    // --- Second Byte ---
    timeout = 0;
    while (!(USART1->ISR & USART_ISR_RXNE)) {
        if (++timeout >= timeoutLimit) {
					taskEXIT_CRITICAL();
					return -2;
				}
    }
    uint8_t lsb = USART1->RDR;

    *outVal = ((uint16_t)msb << 8) | lsb;
		
		taskEXIT_CRITICAL();
    return 0;
}