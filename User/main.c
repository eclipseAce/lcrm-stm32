#include "main.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

volatile uint32_t CurrentMillis;
//volatile uint32_t ButtonPressedMillis[2];
//volatile uint32_t ButtonStatus;

uint32_t LastSentMillis;

uint8_t PGAGain_I = 0;
uint8_t PGAGain_V = 0;

GPIO_InitTypeDef gpioInit;
TIM_TimeBaseInitTypeDef timTimeBaseInit;
TIM_OCInitTypeDef timOCInit;
DAC_InitTypeDef dacInit;
DMA_InitTypeDef dmaInit;
COMP_InitTypeDef compInit;
SPI_InitTypeDef spiInit;
EXTI_InitTypeDef extiInit;
NVIC_InitTypeDef nvicInit;
USART_InitTypeDef usartInit;
ADC_InitTypeDef adcInit;
ADC_CommonInitTypeDef adcCommonInit;

//void InitLED() {
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
//
//  GPIO_StructInit(&gpioInit);
//  gpioInit.GPIO_Pin = GPIO_Pin_13;
//  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
//  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
//  gpioInit.GPIO_OType = GPIO_OType_PP;
//  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
//  GPIO_Init(GPIOC, &gpioInit);
//}

//void InitButton() {
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//
//  GPIO_StructInit(&gpioInit);
//  gpioInit.GPIO_Mode = GPIO_Mode_IN;
//  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
//  gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
//  GPIO_Init(GPIOB, &gpioInit);
//
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
//
//  EXTI_StructInit(&extiInit);
//  extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
//  extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
//  extiInit.EXTI_LineCmd = ENABLE;
//  extiInit.EXTI_Line = EXTI_Line8;
//  EXTI_Init(&extiInit);
//  extiInit.EXTI_Line = EXTI_Line9;
//  EXTI_Init(&extiInit);
//
//  nvicInit.NVIC_IRQChannel = EXTI9_5_IRQn;
//  nvicInit.NVIC_IRQChannelPreemptionPriority = 15;
//  nvicInit.NVIC_IRQChannelSubPriority = 15;
//  nvicInit.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&nvicInit);
//}

int main(void) {
  SysTick_Config(SystemCoreClock / 1000);

//  InitLED();
//  InitButton();
  InitDACSineSignal();
//  InitOrthogonalPWMSignal();
//  InitSPIForOLED();
  InitSPIForPGA();
  InitSerial();
  InitADCSampler();

//  OLED_Init();
//  OLED_Fill(0);
//  OLED_DrawString(0, 0, "Hello world");
  PGA_SetGain(PGA_TARGET_I, 6);
  PGA_SetGain(PGA_TARGET_V, 6);
  while (1) {
//    if (stricmp(input, "GainI") == 0) {
//      PGAGain_I = (PGAGain_I + 1) % 8;
//      PGA_SetGain(PGA_TARGET_I, PGAGain_I);
//    }
//    if (stricmp(input, "GainV") == 0) {
//      PGAGain_V = (PGAGain_V + 1) % 8;
//      PGA_SetGain(PGA_TARGET_V, PGAGain_V);
//    }
//    if (CurrentMillis - LastSentMillis > 1000) {
//      LastSentMillis = CurrentMillis;
//      printf("GainI:%d, GainV:%d\r\n", PGAGain_I, PGAGain_V);
//    }

    int size;
    int16_t *adciValue, *adcvValue;
    ADC_RequestData(&adciValue, &adcvValue, &size);
    int32_t min = INT32_MAX, max = INT32_MIN;
    int32_t imin = INT32_MAX, imax = INT32_MIN;
    int32_t vmin = INT32_MAX, vmax = INT32_MIN;
    for (int i = 0; i < size; i++) {
      int32_t v = (int32_t) ((double) adciValue[i] * adcvValue[i]);
      if (v > max) {
        max = v;
      }
      if (v < min) {
        min = v;
      }
      if (adciValue[i] > imax) {
        imax = adciValue[i];
      }
      if (adciValue[i] < imin) {
        imin = adciValue[i];
      }
      if (adcvValue[i] > vmax) {
        vmax = adcvValue[i];
      }
      if (adcvValue[i] < vmin) {
        vmin = adcvValue[i];
      }
    }
    int32_t amp = (max - min) / 2;
    int32_t mid = (max - min) / 2 + min;
    double theta = acos(2.0 * mid / (amp * 2));

    double z = (double) (vmax - vmin) * 120 / (imax - imin);

    double c = 1000000000 / (2 * M_PI * SINE_SIGNAL_FREQUENCY * z * sin(theta));

    double l = 1000000 * z * sin(theta) / (2 * M_PI * SINE_SIGNAL_FREQUENCY);

    for (int i = 0; i < size; i++) {
      printf("0,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
          (int32_t) ((double) adciValue[i] * adcvValue[i]),
          (int32_t) mid,
          (int32_t) ((double) adciValue[i]),
          (int32_t) ((double) adcvValue[i]),
          (int32_t) (theta * 180 / M_PI),
          (int32_t) z,
          (int32_t) c,
          (int32_t) l);
    }
    Delay(1000);
  }
}

void Delay(uint32_t millis) {
  for (uint32_t m = CurrentMillis; CurrentMillis - m < millis;) {
  }
}

void SysTick_Handler() {
  CurrentMillis++;
}

//void EXTI9_5_IRQHandler() {
//  if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
//    ButtonStatus |= 0x01;
//    EXTI_ClearITPendingBit(EXTI_Line8);
//  } else if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
//    ButtonStatus |= 0x02;
//    EXTI_ClearITPendingBit(EXTI_Line9);
//  }
//}
