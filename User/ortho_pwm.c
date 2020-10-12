#include "main.h"

void InitOrthogonalPWMSignal() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;

  gpioInit.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &gpioInit);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);

  gpioInit.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOB, &gpioInit);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_2);

  uint32_t timerPeriod = SystemCoreClock / (ORTHOGONAL_PWM_FREQUENCY << 1) - 1;

  TIM_TimeBaseStructInit(&timTimeBaseInit);
  timTimeBaseInit.TIM_Prescaler = 0;
  timTimeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
  timTimeBaseInit.TIM_Period = timerPeriod;
  timTimeBaseInit.TIM_ClockDivision = 0;
  timTimeBaseInit.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timTimeBaseInit);

  TIM_OCStructInit(&timOCInit);
  timOCInit.TIM_OCMode = TIM_OCMode_Toggle;
  timOCInit.TIM_OutputState = TIM_OutputState_Enable;
  timOCInit.TIM_OCNIdleState = TIM_OCNIdleState_Set;
  timOCInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
  timOCInit.TIM_OutputNState = TIM_OutputNState_Disable;
  timOCInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
  timOCInit.TIM_OCPolarity = TIM_OCPolarity_Low;

  timOCInit.TIM_Pulse = 0;
  TIM_OC1Init(TIM3, &timOCInit);

  timOCInit.TIM_Pulse = (timerPeriod - 1) * 5 / 10;
  TIM_OC2Init(TIM3, &timOCInit);

  TIM_Cmd(TIM3, ENABLE);
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
}
