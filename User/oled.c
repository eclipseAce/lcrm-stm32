#include "oled.h"
#include "oled_fonts.h"

void OLED_SetPosition(uint8_t x, uint8_t y) {
  OLED_WriteCommand(0xb0 + y);
  OLED_WriteCommand(((x & 0xf0) >> 4) | 0x10);
  OLED_WriteCommand((x & 0x0f) | 0x00);
}

void OLED_WriteData(uint8_t byte) {
  OLED_WriteDC(1);
  OLED_Write(byte);
}

void OLED_WriteCommand(uint8_t byte) {
  OLED_WriteDC(0);
  OLED_Write(byte);
}

void OLED_Fill(uint8_t color) {
  for (int i = 0; i < 8; i++) {
    OLED_WriteCommand(0xb0 + i);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x10);
    for (int j = 0; j < 128; j++) {
      OLED_WriteData(color ? 0xFF : 0x00);
    }
  }
}

void OLED_DrawChar(uint8_t x, uint8_t y, char ch) {
  OLED_SetPosition(x, y * 2);
  for (int i = 0; i < 8; i++) {
    OLED_WriteData(F8X16[ch - ' '][i]);
  }
  OLED_SetPosition(x, y * 2 + 1);
  for (int i = 0; i < 8; i++) {
    OLED_WriteData(F8X16[ch - ' '][8 + i]);
  }
}

void OLED_DrawString(uint8_t x, uint8_t y, const char *str) {
  while (*str) {
    if (x > 127 - 8) {
      x = 0;
      y++;
    }
    OLED_DrawChar(x, y, *str++);
    x += 8;
  }
}

void OLED_Init() {
  OLED_WriteRST(1);
  OLED_Delay(100);
  OLED_WriteRST(0);
  OLED_Delay(100);
  OLED_WriteRST(1);

  OLED_WriteCommand(0xAE);     // Display Off (0x00)
  OLED_WriteCommand(0xD5);
  OLED_WriteCommand(0x80);       // Set Clock as 100 Frames/Sec
  OLED_WriteCommand(0xA8);
  OLED_WriteCommand(0x3F);         // 1/64 Duty (0x0F~0x3F)
  OLED_WriteCommand(0xD3);
  OLED_WriteCommand(0x00);       // Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WriteCommand(0x40 | 0x00);  // Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WriteCommand(0x8D);
  OLED_WriteCommand(0x10 | 0x04);  // Enable Embedded DC/DC Converter (0x00/0x04)
  OLED_WriteCommand(0x20);
  OLED_WriteCommand(0x02);       // Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WriteCommand(0xA0 | 0x01);  // Set SEG/Column Mapping
  OLED_WriteCommand(0xC8);  // Set COM/x Scan Direction
  OLED_WriteCommand(0xDA);
  OLED_WriteCommand(0x02 | 0x10);  // Set Sequential Configuration (0x00/0x10)
  OLED_WriteCommand(0x81);
  OLED_WriteCommand(0xCF);         // Set SEG Output Current
  OLED_WriteCommand(0xD9);
  OLED_WriteCommand(0xF1);         // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WriteCommand(0xDB);
  OLED_WriteCommand(0x40);         // Set VCOM Deselect Level
  OLED_WriteCommand(0xA4 | 0x00);  // Disable Entire Display On (0x00/0x01)
  OLED_WriteCommand(0xA6 | 0x00);  // Disable Inverse Display On (0x00/0x01)
  OLED_WriteCommand(0xAE | 0x01);  // Display On (0x01)
}
