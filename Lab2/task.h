#ifndef TASK_H
#define TASK_H
#include <stdint.h>

extern const uint16_t sinLUT[];
extern volatile int LEDState;
void LEDTask (void *pvParameters);
void BTNTask (void *pvParameters);
//void SINTask (void *pvParameters);
void TIM4_IRQHandler(void);
void DACSetup(void);
void TIM4Setup(void);

#endif