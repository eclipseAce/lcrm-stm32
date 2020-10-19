#include "main.h"

typedef struct {
  ADC_TypeDef *ADCx;
  DMA_Channel_TypeDef *DMAxChannely;
  uint32_t DMAxChannelyGLFlag;
  uint32_t DMAxChannelyTCFlag;
  uint32_t DMAxChannelyHTFlag;
  uint32_t DMAxChannelyTEFlag;
  uint8_t Channelx;
  GPIO_TypeDef *ChannelGPIOx;
  uint16_t ChannelGPIOPortx;
  uint32_t CalibrationValue;
} ADCObject;

ADCObject ADCI = {
    .ADCx = ADC3,
    .DMAxChannely = DMA2_Channel5,
    .DMAxChannelyGLFlag = DMA2_FLAG_GL5,
    .DMAxChannelyTCFlag = DMA2_FLAG_TC5,
    .DMAxChannelyHTFlag = DMA2_FLAG_HT5,
    .DMAxChannelyTEFlag = DMA2_FLAG_TE5,
    .Channelx = ADC_Channel_5,
    .ChannelGPIOx = GPIOB,
    .ChannelGPIOPortx = GPIO_Pin_13
};
ADCObject ADCV = {
    .ADCx = ADC4,
    .DMAxChannely = DMA2_Channel2,
    .DMAxChannelyGLFlag = DMA2_FLAG_GL2,
    .DMAxChannelyTCFlag = DMA2_FLAG_TC2,
    .DMAxChannelyHTFlag = DMA2_FLAG_HT2,
    .DMAxChannelyTEFlag = DMA2_FLAG_TE2,
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
  dmaInit.DMA_MemoryBaseAddr = 0;
  dmaInit.DMA_BufferSize = 0;
  dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
  dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmaInit.DMA_Mode = DMA_Mode_Normal;
  dmaInit.DMA_Priority = DMA_Priority_High;
  dmaInit.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(obj->DMAxChannely, &dmaInit);

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
  adcInit.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_11;
  adcInit.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
  adcInit.ADC_DataAlign = ADC_DataAlign_Right;
  adcInit.ADC_OverrunMode = ADC_OverrunMode_Enable;
  adcInit.ADC_AutoInjMode = ADC_AutoInjec_Disable;
  adcInit.ADC_NbrOfRegChannel = 1;
  ADC_Init(obj->ADCx, &adcInit);
  ADC_RegularChannelConfig(obj->ADCx, obj->Channelx, 1, ADC_SampleTime_1Cycles5);
  ADC_DMAConfig(obj->ADCx, ADC_DMAMode_OneShot);
  ADC_DMACmd(obj->ADCx, ENABLE);
  ADC_Cmd(obj->ADCx, ENABLE);
  while (ADC_GetFlagStatus(obj->ADCx, ADC_FLAG_RDY) == RESET) {
  }
}

static void ADCTrigInit() {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseStructInit(&timTimeBaseInit);
  timTimeBaseInit.TIM_Period = 0;
  timTimeBaseInit.TIM_Prescaler = 0;
  timTimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
  timTimeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
  timTimeBaseInit.TIM_RepetitionCounter = 0x0000;
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Enable);
  TIM_TimeBaseInit(TIM3, &timTimeBaseInit);
}

static void ADCRSELInit() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOB, &gpioInit);
}

void InitADCSampler() {
  RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div1);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  ADCRSELInit();
  ADCTrigInit();
  ADCInit(&ADCI);
  ADCInit(&ADCV);
}

static void ADC_Prepare(ADCObject *obj, int16_t *data, int size) {
  obj->DMAxChannely->CMAR = (uint32_t) data;
  obj->DMAxChannely->CNDTR = size;

  ADC_ClearFlag(obj->ADCx, ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR);
  ADC_StartConversion(obj->ADCx);

  DMA_ClearFlag(obj->DMAxChannelyTCFlag | ADCI.DMAxChannelyHTFlag | ADCI.DMAxChannelyTEFlag);
  DMA_Cmd(obj->DMAxChannely, ENABLE);
}

static void ADC_Finish(ADCObject *obj, int16_t *data, int size) {
  ADC_StopConversion(obj->ADCx);
  DMA_Cmd(obj->DMAxChannely, DISABLE);
}

void ADC_RequestData(RSEL_Value rsel, PGA_Gain igain, PGA_Gain vgain, int16_t *idata, int16_t *vdata, int size) {
  GPIO_WriteBit(GPIOB, GPIO_Pin_8, (rsel & 0x01) ? Bit_SET : Bit_RESET);
  GPIO_WriteBit(GPIOB, GPIO_Pin_9, (rsel & 0x02) ? Bit_SET : Bit_RESET);

  PGA_SetGain(PGA_TARGET_I, igain);
  PGA_SetGain(PGA_TARGET_V, vgain);

  Delay(10);

  ADC_Prepare(&ADCI, idata, size);
  ADC_Prepare(&ADCV, vdata, size);
  TIM_Cmd(TIM3, ENABLE);
  while (DMA_GetFlagStatus(ADCI.DMAxChannelyTCFlag) == RESET
      || DMA_GetFlagStatus(ADCV.DMAxChannelyTCFlag) == RESET) {
  }
  TIM_Cmd(TIM3, DISABLE);
  ADC_Finish(&ADCI, idata, size);
  ADC_Finish(&ADCV, vdata, size);
}
