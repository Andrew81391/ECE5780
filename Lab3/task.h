#ifndef TASK_H
#define TASK_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueueLED;
extern const uint16_t sinLUT[];
void LEDTask (void *pvParameters);
void BTNTask (void *pvParameters);
void TIM4_IRQHandler(void);
void DACSetup(void);
void TIM4Setup(void);

#endif
