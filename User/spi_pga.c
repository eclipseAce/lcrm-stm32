#include "main.h"

void InitSPIForPGA() {
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_AF;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
  GPIO_Init(GPIOB, &gpioInit);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_6);

  GPIO_StructInit(&gpioInit);
  gpioInit.GPIO_Mode = GPIO_Mode_OUT;
  gpioInit.GPIO_OType = GPIO_OType_PP;
  gpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
  gpioInit.GPIO_Speed = GPIO_Speed_Level_3;
  gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOB, &gpioInit);

  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  GPIO_SetBits(GPIOB, GPIO_Pin_7);

  SPI_StructInit(&spiInit);
  spiInit.SPI_CPOL = SPI_CPOL_Low;
  spiInit.SPI_CPHA = SPI_CPHA_1Edge;
  spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  spiInit.SPI_DataSize = SPI_DataSize_16b;
  spiInit.SPI_Direction = SPI_Direction_1Line_Tx;
  spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
  spiInit.SPI_Mode = SPI_Mode_Master;
  spiInit.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI3, &spiInit);
  SPI_Cmd(SPI3, ENABLE);
}

void PGA_SetGain(PGA_Target target, PGA_Gain gain) {
  uint16_t pin;
  switch (target) {
  case PGA_TARGET_I:
    pin = GPIO_Pin_6;
    break;
  case PGA_TARGET_V:
    pin = GPIO_Pin_7;
    break;
  }
  GPIO_ResetBits(GPIOB, pin);
  Delay(10);
  SPI_I2S_SendData16(SPI3, 0x4000 | (gain & 0x07));
  Delay(10);
  GPIO_SetBits(GPIOB, pin);
}
