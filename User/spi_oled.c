#include "main.h"
#include "oled.h"

void InitSPIForOLED() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &gpioInit);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_5);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIOB, &gpioInit);

  SPI_StructInit(&spiInit);
  spiInit.SPI_CPOL = SPI_CPOL_High;
  spiInit.SPI_CPHA = SPI_CPHA_1Edge;
  spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  spiInit.SPI_DataSize = SPI_DataSize_8b;
  spiInit.SPI_Direction = SPI_Direction_1Line_Tx;
  spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
  spiInit.SPI_Mode = SPI_Mode_Master;
  spiInit.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI1, &spiInit);
  SPI_Cmd(SPI1, ENABLE);
}

void OLED_Delay(uint16_t millis) {
  Delay(millis);
}

void OLED_Write(uint8_t value) {
  SPI_SendData8(SPI1, value);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
  }
}

void OLED_WriteDC(uint8_t value) {
  if (value) {
    GPIO_SetBits(GPIOB, GPIO_Pin_4);
  } else {
    GPIO_ResetBits(GPIOB, GPIO_Pin_4);
  }
}

void OLED_WriteRST(uint8_t value) {
  if (value) {
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
  } else {
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
  }
}
