#ifndef OURTASK_H
#define OURTASK_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueueLED;
extern TaskHandle_t LEDTaskHandle;
extern TaskHandle_t BTNTaskHandle;
extern TaskHandle_t sensorTaskHandle;
extern const uint16_t sinLUT[];
void LEDTask (void *pvParameters);
void BTNTask (void *pvParameters);
void SENSORTask (void *pvParameters);
void TIM4_IRQHandler(void);
void DACSetup(void);
void TIM4Setup(void);
int32_t UART3_Read2Bytes(uint16_t *outVal, int32_t timeoutLimit, char command);

#endif
