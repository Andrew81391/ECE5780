#ifndef OURTASK_H
#define OURTASK_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

extern QueueHandle_t xQueueLED;
extern QueueHandle_t xQueueVOL;
extern TaskHandle_t LEDTaskHandle;
extern TaskHandle_t BTNTaskHandle;
extern TaskHandle_t sensorTaskHandle;
extern TaskHandle_t FREQTaskHandle;
extern SemaphoreHandle_t uartMutex;
extern StaticSemaphore_t uartMutexBuffer;
extern const uint16_t sinLUT[];
void LEDTask (void *pvParameters);
void BTNTask (void *pvParameters);
void FREQTask (void *pvParameters);
void VOLTask (void *pvParameters);
void SENSORTask (void *pvParameters);
void TIM4_IRQHandler(void);
void DACSetup(void);
void TIM4Setup(void);
int32_t UART3_Read2Bytes(uint16_t *outVal, int32_t timeoutLimit, char command);
int32_t UART1_Read2Bytes(uint16_t *outVal, int32_t timeoutLimit, char command);

typedef enum {
    C3, Cs3, D3, Ds3, E3, F3, Fs3, G3, Gs3, A3, As3, B3,
    C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
    C5
} Note;

typedef enum {
	V0, V1, V2, V3, V4, V5, V6, V7
} Volume;

#endif
