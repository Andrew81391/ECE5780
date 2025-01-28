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
			for (volatile int i = 0; i < 45000; i++);
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
		DAC->DHR12R1 &= 0xFFFFF000;
		DAC->DHR12R1 |= sinLUT[index];
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
		
		//increment the lookup table index
		index += LEDState;
		if (index >= 64) {
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
	TIM4->PSC = 7;
	TIM4->ARR = 70;
	
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