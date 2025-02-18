#pragma once
#include "stm32l476xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>
#include <stdio.h>
extern QueueHandle_t xQueueUART;
void USART_Init_PC (void);
void USART_Init_US100 (void);
void USART2_IRQHandler(void);
