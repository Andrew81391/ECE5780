#include "stm32l476xx.h"
#include <stdint.h>
#include <stdio.h>
#include "UART.h"
#include "FreeRTOS.h"
#include "queue.h"

/*
void USART2_IRQHandler(void){
	if ((USART2->ISR & USART_ISR_RXNE)){
		char note = (char)USART2->RDR;
//		if ((note == 'p')) {
//			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//			BaseType_t status = xQueueSendToBackFromISR(xQueueSENSOR, &note, 0);
//			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//		}
		//else {
			xQueueSendToBackFromISR(xQueueUART, &note, NULL);
		//}
	}
}
*/

void USART_Init_PC (void) { 
	//set PA2,PA3 to alt function
	GPIOA->MODER &= 0xFFFFFF0F;
	GPIOA->MODER |= 0x000000A0;
   //set PB10,PB11 to alt function
	GPIOB->MODER &= 0xFF0FFFFF;
	GPIOB->MODER |= 0x00A00000;
	//set PB6,PB7 to alt function
	GPIOB->MODER &= 0xFFFF0FFF;
	GPIOB->MODER |= 0x0000A000;
	//set alt function USART2
	GPIOA->AFR[0] &= 0xFFFF00FF;
	GPIOA->AFR[0] |= 0x00007700;
	//set alt function USART 3
	GPIOB->AFR[1] &= 0xFFFF00FF;
	GPIOB->AFR[1] |= 0x00007700;
	//set alt function USART 1
	GPIOB->AFR[0] &= 0x00FFFFFF;
	GPIOB->AFR[0] |= 0x77000000;
	//set speed USART2
	GPIOA->OSPEEDR |= 0x000000F0;
	//set speed USART3
	GPIOB->OSPEEDR |= 0x00F00000;
	//set speed USART1
	GPIOB->OSPEEDR |= 0x0000F000;
	//set pull up usart2
	GPIOA->PUPDR &= 0xFFFFFF0F;
	GPIOA->PUPDR |= 0x00000050;
	//set pull up usart3
	GPIOB->PUPDR &= 0xFF0FFFFF;
	GPIOB->PUPDR |= 0x00500000;
	//set pull up usart1
	GPIOB->PUPDR &= 0xFFFF0FFF;
	GPIOB->PUPDR |= 0x00005000;
	//enable USART2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	//enable USART3 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
	//enable usart1 clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	//selectUSART2 as USART bus
	RCC->CCIPR &= ~ (RCC_CCIPR_USART2SEL);
	RCC->CCIPR |= (RCC_CCIPR_USART2SEL_0);
	//selectUSART3 as USART bus
	RCC->CCIPR &= ~ (RCC_CCIPR_USART3SEL);
	RCC->CCIPR |= (RCC_CCIPR_USART3SEL_0);
	//select usart1 as usart bus
	RCC->CCIPR &= ~ (RCC_CCIPR_USART1SEL);
	RCC->CCIPR |= (RCC_CCIPR_USART1SEL_0);
	//uart interrupts RX
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);
	// Disable USART2
	USART2->CR1 &= ~USART_CR1_UE; 
	// Disable USART3
	USART3->CR1 &= ~USART_CR1_UE; 
	//Disable USART1
	USART1->CR1 &= ~USART_CR1_UE;
	// Set data Length to 8 bits for usart2
	// ee = B data bits, e1 = 9 data bits, 1e = 7 data bits 
	USART2->CR1 &= ~USART_CR1_M; 
	// Set data Length to 8 bits for usart3
	// ee = B data bits, e1 = 9 data bits, 1e = 7 data bits 
	USART3->CR1 &= ~USART_CR1_M;
	// Set data Length to 8 bits for usart1
	// ee = B data bits, e1 = 9 data bits, 1e = 7 data bits 
	USART1->CR1 &= ~USART_CR1_M; 
	//	Select 1 stop bit for usart2
	// ee = 1 stop bit e1 = e.s stop bit 
	// 1e = 2 Stop bits 11 = 1.5 Stop bit 
	USART2->CR2 &= ~USART_CR2_STOP; 
	//	Select 1 stop bit for usart3
	// ee = 1 stop bit e1 = e.s stop bit 
	// 1e = 2 Stop bits 11 = 1.5 Stop bit 
	USART3->CR2 &= ~USART_CR2_STOP; 
	//	Select 1 stop bit for usart1
	// ee = 1 stop bit e1 = e.s stop bit 
	// 1e = 2 Stop bits 11 = 1.5 Stop bit 
	USART1->CR2 &= ~USART_CR2_STOP; 
	// Set parity control as no parity for usart2
	//	e = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USART2->CR1 &= ~USART_CR1_PCE; 
	// Set parity control as no parity for usart3
	//	e = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USART3->CR1 &= ~USART_CR1_PCE; 
	// Set parity control as no parity for usart1
	//	e = no parity, 
	// 1 = parity enabled (then, program PS bit to select Even or Odd parity) 
	USART1->CR1 &= ~USART_CR1_PCE; 
	// Oversampling by 16 for usart2
	// 0 =oversampling by 16, 1 =oversampling by 8 
	USART2->CR1 &= ~USART_CR1_OVER8; 
	// Oversampling by 16 for usart3
	// 0 =oversampling by 16, 1 =oversampling by 8 
	USART3->CR1 &= ~USART_CR1_OVER8; 
	// Oversampling by 16 for usart1
	// 0 =oversampling by 16, 1 =oversampling by 8 
	USART1->CR1 &= ~USART_CR1_OVER8; 
	// Set Baud rate to 9600 using APB frequency (Be MHz) for usart2
	// See Example 1 in Section 22.1.2 
	USART2->BRR = 0x341; 
	// Set Baud rate to 9600 using APB frequency (Be MHz) for usart3
	// See Example 1 in Section 22.1.2 
	USART3->BRR = 0x683; 
	// Set Baud rate to 9600 using APB frequency (Be MHz) for usart1
	// See Example 1 in Section 22.1.2 
	USART1->BRR = 0x683; 
	// Enable transmission and reception for usart2
	USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	// Enable transmission and reception for usart3
	USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	// Enable transmission and reception for usart1
	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE); 
	// Enable USART2 
	USART2->CR1 |= USART_CR1_UE; 
	// Enable USART3 
	USART3->CR1 |= USART_CR1_UE; 
	// Enable USART1
	USART1->CR1 |= USART_CR1_UE; 
	// Verify that USART2 is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USART2->ISR & USART_ISR_TEACK) == 0);
	// Verify that USART3 is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USART3->ISR & USART_ISR_TEACK) == 0); 	
	// Verify that USART1 is ready for transmission 
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it. 
	while ((USART1->ISR & USART_ISR_TEACK) == 0); 	
	// Verify that USART2 is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USART2->ISR & USART_ISR_REACK) == 0); 
	// Verify that USART3 is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USART3->ISR & USART_ISR_REACK) == 0); 
	// Verify that USART1 is ready for reception 
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it. 
	while ((USART1->ISR & USART_ISR_REACK) == 0); 
}

//use USART3 on PB10,PB11
void USART_Init_US100 (void){
	
	
}