#include "main.h"

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

volatile uint32_t CurrentMillis;
volatile uint32_t ButtonPressedMillis[2];
volatile uint32_t ButtonStatus;

void InitLED() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Pin = GPIO_Pin_13;
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &gpioInit);
}

void InitButton() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_IN;
  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOB, &gpioInit);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);

  EXTI_StructInit(&extiInit);
  extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
  extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
  extiInit.EXTI_LineCmd = ENABLE;
  extiInit.EXTI_Line = EXTI_Line8;
  EXTI_Init(&extiInit);
  extiInit.EXTI_Line = EXTI_Line9;
  EXTI_Init(&extiInit);

  nvicInit.NVIC_IRQChannel = EXTI9_5_IRQn;
  nvicInit.NVIC_IRQChannelPreemptionPriority = 15;
  nvicInit.NVIC_IRQChannelSubPriority = 15;
  nvicInit.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicInit);
}

void Delay(uint32_t millis) {
  for (uint32_t m = CurrentMillis; CurrentMillis - m < millis;) {
  }
}

void SysTick_Handler() {
  CurrentMillis++;
}

void EXTI9_5_IRQHandler() {
  if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
    ButtonStatus |= 0x01;
    EXTI_ClearITPendingBit(EXTI_Line8);
  } else if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
    ButtonStatus |= 0x02;
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}
