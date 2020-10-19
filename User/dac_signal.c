#include "main.h"
#include <math.h>

#define SINE_TABLE_SIZE 72

uint16_t Sine12bit[SINE_TABLE_SIZE];

static void GenerateSineData(uint16_t *data, int size, int n, double scale) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < size; j++) {
      data[i * size + j] = (uint16_t) (sin(M_PI * 2 * j / size) * 2047.5 * scale + 2047.5);
    }
  }
}

void InitDACSineSignal() {
  GenerateSineData(Sine12bit, SINE_TABLE_SIZE, 1, 0.6);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_DMAChannelRemapConfig(SYSCFG_DMARemap_TIM6DAC1Ch1, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Pin = GPIO_Pin_4;
  gpioInit.GPIO_Mode = GPIO_Mode_AN;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &gpioInit);

  TIM_TimeBaseStructInit(&timTimeBaseInit);
  timTimeBaseInit.TIM_Period = SystemCoreClock / SINE_SIGNAL_FREQUENCY / SINE_TABLE_SIZE - 1;
  timTimeBaseInit.TIM_Prescaler = 0;
  timTimeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
  timTimeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &timTimeBaseInit);
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
  TIM_Cmd(TIM2, ENABLE);

  DAC_StructInit(&dacInit);
  dacInit.DAC_Trigger = DAC_Trigger_T2_TRGO;
  dacInit.DAC_WaveGeneration = DAC_WaveGeneration_None;
  dacInit.DAC_Buffer_Switch = DAC_BufferSwitch_Disable;
  DAC_Init(DAC, DAC_Channel_1, &dacInit);
  DAC_Cmd(DAC, DAC_Channel_1, ENABLE);

  DMA_StructInit(&dmaInit);
  dmaInit.DMA_PeripheralBaseAddr = DAC_BASE + 0x08;
  dmaInit.DMA_MemoryBaseAddr = (uint32_t) &Sine12bit;
  dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
  dmaInit.DMA_BufferSize = SINE_TABLE_SIZE;
  dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmaInit.DMA_Mode = DMA_Mode_Circular;
  dmaInit.DMA_Priority = DMA_Priority_High;
  dmaInit.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel3, &dmaInit);
  DMA_Cmd(DMA1_Channel3, ENABLE);
  DAC_DMACmd(DAC, DAC_Channel_1, ENABLE);
}
