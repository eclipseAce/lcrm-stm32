#include "main.h"

#define ADC_DATA_SIZE 2000

typedef struct {
  ADC_TypeDef *ADCx;
  DMA_Channel_TypeDef *DMAxChannely;
  uint32_t DMAxChannelyTCFlag;
  uint8_t Channelx;
  GPIO_TypeDef *ChannelGPIOx;
  uint16_t ChannelGPIOPortx;
  uint32_t CalibrationValue;
  int16_t Values[ADC_DATA_SIZE];
} ADCObject;

ADCObject ADCI = {
    .ADCx = ADC3,
    .DMAxChannely = DMA2_Channel5,
    .DMAxChannelyTCFlag = DMA2_FLAG_TC5,
    .Channelx = ADC_Channel_5,
    .ChannelGPIOx = GPIOB,
    .ChannelGPIOPortx = GPIO_Pin_13
};
ADCObject ADCV = {
    .ADCx = ADC4,
    .DMAxChannely = DMA2_Channel2,
    .DMAxChannelyTCFlag = DMA2_FLAG_TC2,
    .Channelx = ADC_Channel_3,
    .ChannelGPIOx = GPIOB,
    .ChannelGPIOPortx = GPIO_Pin_12
};

static void ADCInit(ADCObject *obj) {
  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_AN;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInit.GPIO_Pin = obj->ChannelGPIOPortx;
  GPIO_Init(obj->ChannelGPIOx, &gpioInit);

  DMA_StructInit(&dmaInit);
  dmaInit.DMA_PeripheralBaseAddr = (uint32_t) obj->ADCx + 0x40;
  dmaInit.DMA_MemoryBaseAddr = (uint32_t) &obj->Values;
  dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
  dmaInit.DMA_BufferSize = ADC_DATA_SIZE;
  dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmaInit.DMA_Mode = DMA_Mode_Circular;
  dmaInit.DMA_Priority = DMA_Priority_High;
  dmaInit.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(obj->DMAxChannely, &dmaInit);
  DMA_Cmd(obj->DMAxChannely, ENABLE);

  ADC_VoltageRegulatorCmd(obj->ADCx, ENABLE);
  Delay(10);
  ADC_SelectCalibrationMode(obj->ADCx, ADC_CalibrationMode_Single);
  ADC_StartCalibration(obj->ADCx);
  while (ADC_GetCalibrationStatus(obj->ADCx) != RESET) {
  }
  obj->CalibrationValue = ADC_GetCalibrationValue(obj->ADCx);

  ADC_CommonStructInit(&adcCommonInit);
  adcCommonInit.ADC_Mode = ADC_Mode_Independent;
  adcCommonInit.ADC_Clock = ADC_Clock_AsynClkMode;
  adcCommonInit.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  adcCommonInit.ADC_DMAMode = ADC_DMAMode_OneShot;
  adcCommonInit.ADC_TwoSamplingDelay = 0;
  ADC_CommonInit(obj->ADCx, &adcCommonInit);

  ADC_StructInit(&adcInit);
  adcInit.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Enable;
  adcInit.ADC_Resolution = ADC_Resolution_12b;
  adcInit.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
  adcInit.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
  adcInit.ADC_DataAlign = ADC_DataAlign_Right;
  adcInit.ADC_OverrunMode = ADC_OverrunMode_Disable;
  adcInit.ADC_AutoInjMode = ADC_AutoInjec_Disable;
  adcInit.ADC_NbrOfRegChannel = 1;
  ADC_Init(obj->ADCx, &adcInit);
  ADC_RegularChannelConfig(obj->ADCx, obj->Channelx, 1, ADC_SampleTime_1Cycles5);
  ADC_Cmd(obj->ADCx, ENABLE);
  while (!ADC_GetFlagStatus(obj->ADCx, ADC_FLAG_RDY)) {
  }
  ADC_DMAConfig(obj->ADCx, ADC_DMAMode_OneShot);
  ADC_DMACmd(obj->ADCx, ENABLE);
}

static void ADCTrigInit() {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&timTimeBaseInit);
  timTimeBaseInit.TIM_Prescaler = 0;
  timTimeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
  timTimeBaseInit.TIM_Period = SystemCoreClock / 1000 - 1;
  timTimeBaseInit.TIM_ClockDivision = 0;
  timTimeBaseInit.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &timTimeBaseInit);

  TIM_OCStructInit(&timOCInit);
  timOCInit.TIM_Pulse = SystemCoreClock / 1000 - 1;
  timOCInit.TIM_OCMode = TIM_OCMode_Toggle;
  timOCInit.TIM_OutputState = TIM_OutputState_Enable;
  timOCInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
  timOCInit.TIM_OCPolarity = TIM_OCPolarity_Low;
  timOCInit.TIM_OutputNState = TIM_OutputNState_Disable;
  timOCInit.TIM_OCNIdleState = TIM_OCNIdleState_Set;
  timOCInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OC1Init(TIM3, &timOCInit);

  TIM_Cmd(TIM3, ENABLE);
}

static void ConvertDCToAC(int16_t *data, int size) {
  int32_t max = 0, min = 0x7FFF;
  for (int i = 0; i < size; i++) {
    if (data[i] > max) {
      max = data[i];
    }
    if (data[i] < min) {
      min = data[i];
    }
  }
  int32_t mid = (max - min) / 2 + min;
  for (int i = 0; i < size; i++) {
    data[i] = (int32_t) data[i] - mid;
  }
}

void InitADCSampler() {
  RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  ADCTrigInit();
  ADCInit(&ADCI);
  ADCInit(&ADCV);
}

void ADC_RequestData(int16_t **ival, int16_t **vval, int *size) {
  ADC_StartConversion(ADCI.ADCx);
  ADC_StartConversion(ADCV.ADCx);
  while (DMA_GetFlagStatus(ADCI.DMAxChannelyTCFlag) == RESET
      || DMA_GetFlagStatus(ADCV.DMAxChannelyTCFlag) == RESET) {
  }
  DMA_ClearFlag(ADCI.DMAxChannelyTCFlag);
  DMA_ClearFlag(ADCV.DMAxChannelyTCFlag);
  ConvertDCToAC(ADCI.Values, ADC_DATA_SIZE);
  ConvertDCToAC(ADCV.Values, ADC_DATA_SIZE);

  *size = ADC_DATA_SIZE;
  *ival = ADCI.Values;
  *vval = ADCV.Values;
}
