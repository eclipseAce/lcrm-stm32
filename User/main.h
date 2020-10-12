#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f30x.h"
#include <stdint.h>

#define SINE_SIGNAL_FREQUENCY     10000
#define ORTHOGONAL_PWM_FREQUENCY  40000

typedef enum {
  PGA_GAIN_1 = 0,
  PGA_GAIN_2,
  PGA_GAIN_4,
  PGA_GAIN_5,
  PGA_GAIN_8,
  PGA_GAIN_10,
  PGA_GAIN_16,
  PGA_GAIN_32
} PGA_Gain;

typedef enum {
  PGA_TARGET_I,
  PGA_TARGET_V
} PGA_Target;

extern GPIO_InitTypeDef gpioInit;
extern TIM_TimeBaseInitTypeDef timTimeBaseInit;
extern TIM_OCInitTypeDef timOCInit;
extern DAC_InitTypeDef dacInit;
extern DMA_InitTypeDef dmaInit;
extern COMP_InitTypeDef compInit;
extern SPI_InitTypeDef spiInit;
extern EXTI_InitTypeDef extiInit;
extern NVIC_InitTypeDef nvicInit;
extern USART_InitTypeDef usartInit;
extern ADC_InitTypeDef adcInit;
extern ADC_CommonInitTypeDef adcCommonInit;

void Delay(uint32_t millis);
void InitDACSineSignal();
void InitOrthogonalPWMSignal();
void InitSPIForOLED();
void InitSPIForPGA();
void InitSerial();
void InitADCSampler();

void PGA_SetGain(PGA_Target target, PGA_Gain gain);

void ADC_RequestData(int16_t **ival, int16_t **vval, int *size);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
