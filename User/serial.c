#include "main.h"
#include <stdio.h>
#include <sys/stat.h>

void InitSerial() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  gpioInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &gpioInit);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

  nvicInit.NVIC_IRQChannel = USART1_IRQn;
  nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
  nvicInit.NVIC_IRQChannelSubPriority = 0;
  nvicInit.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvicInit);

  DMA_StructInit(&dmaInit);
  dmaInit.DMA_PeripheralBaseAddr = USART1_BASE + 0x28;
  dmaInit.DMA_MemoryBaseAddr = 0;
  dmaInit.DMA_BufferSize = 0;
  dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
  dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dmaInit.DMA_Mode = DMA_Mode_Normal;
  dmaInit.DMA_Priority = DMA_Priority_High;
  dmaInit.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &dmaInit);

  DMA_StructInit(&dmaInit);
  dmaInit.DMA_PeripheralBaseAddr = USART1_BASE + 0x24;
  dmaInit.DMA_MemoryBaseAddr = 0;
  dmaInit.DMA_BufferSize = 0;
  dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
  dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dmaInit.DMA_Mode = DMA_Mode_Normal;
  dmaInit.DMA_Priority = DMA_Priority_High;
  dmaInit.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &dmaInit);

  USART_StructInit(&usartInit);
  usartInit.USART_BaudRate = 115200;
  usartInit.USART_WordLength = USART_WordLength_8b;
  usartInit.USART_StopBits = USART_StopBits_1;
  usartInit.USART_Parity = USART_Parity_No;
  usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &usartInit);
  USART_Cmd(USART1, ENABLE);

  USART_ITConfig(USART1, USART_IT_ERR, ENABLE);

  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
//  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}

int _isatty(int fd) {
  return 1;
}

int _write(int fd, char* ptr, int len) {
  DMA1_Channel4->CMAR = (uint32_t) ptr;
  DMA1_Channel4->CNDTR = len;
  DMA_Cmd(DMA1_Channel4, ENABLE);
  while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET) {
  }
  DMA_Cmd(DMA1_Channel4, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC4);
  return len;
}

int _close(int fd) {
  return 0;
}

int _lseek(int fd, int ptr, int dir) {
  return -1;
}

int _read(int fd, char* ptr, int len) {
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {
  }
  ptr[0] = USART_ReceiveData(USART1);
  return 1;
}

int _fstat(int fd, struct stat* st) {
  st->st_mode = S_IFCHR;
  return 0;
}

void USART1_IRQHandler() {
  if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET) {
    USART_ClearFlag(USART1, USART_FLAG_ORE);
  }
}
