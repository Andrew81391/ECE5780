#include "stm32l476xx.h"
#include <stdint.h>
#include <stdio.h>
#include "UART.h"
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueueUART;
extern QueueHandle_t xQueueSENSOR;

void USART2_IRQHandler(void){
	if ((USART2->ISR & USART_ISR_RXNE)){
		char note = (char)USART2->RDR;
		if ((note == 'p') || (note == 't')) {
			BaseType_t status = xQueueSendToBackFromISR(xQueueSENSOR, &note, 0);
			//while (status != pdPASS);
		}
		else {
			xQueueSendToBackFromISR(xQueueUART, &note, 0);
		}
	}
}


void USART_Init_PC (void) { 
	//set PA2,PA3 to alt function
	GPIOA->MODER &= 0xFFFFFF0F;
	GPIOA->MODER |= 0x000000A0;
	//set alt function
	GPIOA->AFR[0] &= 0xFFFF00FF;
	GPIOA->AFR[0] |= 0x00007700;
	//set speed
	GPIOA->OSPEEDR |= 0x000000F0;
	//set pull up
	GPIOA->PUPDR &= 0xFFFFFF0F;
	GPIOA->PUPDR |= 0x00000050;
	//enable UART clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	//selectUSART2 as USART bus
	RCC->CCIPR &= ~ (RCC_CCIPR_USART2SEL);
	RCC->CCIPR |= ~ (RCC_CCIPR_USART2SEL_0);
	//uart interrupts RX
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
	// Disable USART 
	USART2->CR1 &= ~USART_CR1_UE; 
	// Set data Length to 8 bits 
	// ee = B data bits, e1 = 9 data bits, 1e = 7 data bits 
	USART2->CR1 &= ~USART_CR1_M; 
	//	Select 1 stop bit 
	// ee = 1 stop bit e1 = e.s stop bit 
	// 1e = 2 Stop bits 11 = 1.5 Stop bit 
	USART2->CR2 &= ~USART_CR2_STOP; 
	// Set parity control as no parity 
	//	e = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USART2->CR1 &= ~USART_CR1_PCE; 
	// Oversampling by 16 
	// 0 =oversampling by 16, 1 =oversampling by 8 
	USART2->CR1 &= ~USART_CR1_OVER8; 
	// Set Baud rate to 9600 using APB frequency (Be MHz) 
	// See Example 1 in Section 22.1.2 
	USART2->BRR = 0x683; 
	// Enable transmission and reception 
	USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	// Enable USART 
	USART2->CR1 |= USART_CR1_UE; 
	// Verify that USART is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USART2->ISR & USART_ISR_TEACK) == 0); 
	// Verify that USART is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USART2->ISR & USART_ISR_REACK) == 0); 
}

//use USART3 on PB10,PB11
void USART_Init_US100 (void){
   //set PB10,PB11 to alt function
	GPIOB->MODER &= 0xFFF0FFFF;
	GPIOB->MODER |= 0x00050000;
	//set alt function
	GPIOB->AFR[1] &= 0xFFFF00FF;
	GPIOB->AFR[1] |= 0x00007700;
	//set speed
	GPIOB->OSPEEDR |= 0x00F00000;
	//set pull up
	GPIOB->PUPDR &= 0xFF0FFFFF;
	GPIOB->PUPDR |= 0x00500000;
	//enable UART clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	//selectUSART3 as USART bus
	RCC->CCIPR &= ~ (RCC_CCIPR_USART3SEL);
	RCC->CCIPR |= ~ (RCC_CCIPR_USART3SEL_0);
//	//uart interrupts RX
//	USART3->CR1 |= USART_CR1_RXNEIE;
//	NVIC_SetPriority(USART3_IRQn, 0);
//	NVIC_EnableIRQ(USART3_IRQn);
	// Disable USART 
	USART3->CR1 &= ~USART_CR1_UE; 
	// Set data Length to 8 bits 
	// ee = B data bits, e1 = 9 data bits, 1e = 7 data bits 
	USART3->CR1 &= ~USART_CR1_M; 
	//	Select 1 stop bit 
	// ee = 1 stop bit e1 = e.s stop bit 
	// 1e = 2 Stop bits 11 = 1.5 Stop bit 
	USART3->CR2 &= ~USART_CR2_STOP; 
	// Set parity control as no parity 
	//	e = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USART3->CR1 &= ~USART_CR1_PCE; 
	// Oversampling by 16 
	// 0 =oversampling by 16, 1 =oversampling by 8 
	USART3->CR1 &= ~USART_CR1_OVER8; 
	// Set Baud rate to 9600 using APB frequency (Be MHz) 
	// See Example 1 in Section 22.1.2 
	USART3->BRR = 0x683; 
	// Enable transmission and reception 
	USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	// Enable USART 
	USART3->CR1 |= USART_CR1_UE; 
	// Verify that USART is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USART3->ISR & USART_ISR_TEACK) == 0); 
	// Verify that USART is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USART3->ISR & USART_ISR_REACK) == 0); 
}