#ifndef __OLED_H__
#define __OLED_H__

#include <stdint.h>

void OLED_Init();
void OLED_Fill(uint8_t color);
void OLED_WriteCommand(uint8_t byte);
void OLED_WriteData(uint8_t byte);
void OLED_SetPosition(uint8_t x, uint8_t y);
void OLED_DrawChar(uint8_t x, uint8_t y, char ch);
void OLED_DrawString(uint8_t x, uint8_t y, const char *str);

void OLED_Delay(uint16_t millis);
void OLED_Write(uint8_t value);
void OLED_WriteDC(uint8_t value);
void OLED_WriteRST(uint8_t value);

#endif /* __OLED_H__ */
