#include "task.h"
#include "stm32l476xx.h"

extern const uint16_t sinLUT[];

extern volatile int LEDState;
extern volatile int index;

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

void TIM4_IRQHandler() {
	if ((TIM4->SR & TIM_SR_CC1IF) != 0) {
		//set DAC output according to the lookup table
		DAC->DHR12R2 = sinLUT[index];
		
		//increment the lookup table index
		index += LEDState;
		if (index >= 63) {
			index = 0;
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
	DAC->MCR &= ~(7U<<16);
	
	//enable trigger for DAC channel 2
	DAC->CR |= DAC_CR_TEN2;
	
	//clear trigger selection for channel 2
	DAC->CR &= ~DAC_CR_TSEL2;
	
	//select timer 4 as trigger for DAC channel 2
	DAC->CR |= (DAC_CR_TSEL2_0 | DAC_CR_TSEL2_2);
	
	//enable DAC channel 2
	DAC->CR |= DAC_CR_EN2;
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
	
	//set division rate for 440Hz
	TIM4->PSC = 0;
	TIM4->ARR = 141;
	
	//select up counting
	TIM4->CR1 &= ~TIM_CR1_DIR;
	
	//enable update interrupts
	TIM4->DIER |= TIM_DIER_UIE;
	
	//enable timer 4 interrupt
	NVIC_EnableIRQ(TIM4_IRQn);
	
	//enable the counter
	TIM4->CR1 |= TIM_CR1_CEN;
}

/*
void SINTask (void *pvParameters) {
	for ( ;; ) {
		if (LEDState) {
			while ((DAC->SR & DAC_SR_BWST2) != 0);
			DAC->DHR12R2 = sinLUT[index];
			DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;
		}
	}
}
*/