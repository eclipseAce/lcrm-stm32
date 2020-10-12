#include "main.h"
#include <stdio.h>

#define BUFFER_SIZE 128

uint8_t TxBuffer[BUFFER_SIZE];
uint8_t RxBuffer[BUFFER_SIZE];

void InitSerial() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
  gpioInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &gpioInit);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

  USART_StructInit(&usartInit);
  usartInit.USART_BaudRate = 115200;
  usartInit.USART_WordLength = USART_WordLength_8b;
  usartInit.USART_StopBits = USART_StopBits_1;
  usartInit.USART_Parity = USART_Parity_No;
  usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &usartInit);
  USART_Cmd(USART1, ENABLE);
}

int _write(int file, char *ptr, int len) {
  for (int i = 0; i < len; i++) {
    USART_SendData(USART1, ptr[i]);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {
    }
  }
  return len;
}

int _read(int fd, char* ptr, int len) {
  for (int i = 0; i < len; i++) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {
    }
    ptr[i] = (uint8_t) USART_ReceiveData(USART1);
  }
  return len;
}
