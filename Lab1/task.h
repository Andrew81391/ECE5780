#ifndef TASK_H
#define TASK_H

extern volatile int LEDState;
void LEDTask (void *pvParameters);
void BTNTask (void *pvParameters);

#endif