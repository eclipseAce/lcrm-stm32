#include "main.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define MAX_SAMPLE_SIZE  8000

int16_t ISamples[MAX_SAMPLE_SIZE];
int16_t VSamples[MAX_SAMPLE_SIZE];

int main(void) {
  SysTick_Config(SystemCoreClock / 1000);

//  InitLED();
//  InitButton();
  InitDACSineSignal();
//  InitOrthogonalPWMSignal();
//  InitSPIForOLED();
  InitSPIForPGA();
  InitSerial();
  InitADCSampler();

//  OLED_Init();
//  OLED_Fill(0);
//  OLED_DrawString(0, 0, "Hello world");

  while (1) {
    char cmd[30];
    gets(cmd);
    strlwr(cmd);

    if (strstr(cmd, "getsample ") == cmd) {
      uint size, rsel, igain, vgain;
      sscanf(cmd, "getsample %u %u %u %u", &size, &rsel, &igain, &vgain);
      size = MIN(size, MAX_SAMPLE_SIZE);
      rsel = (rsel % 4);
      igain = (igain % 8);
      vgain = (vgain % 8);
      ADC_RequestData(rsel, igain, vgain, ISamples, VSamples, size);
      for (int i = 0; i < size; i++) {
        printf("%d,%d\r\n", ISamples[i], VSamples[i]);
      }
    }
  }
}

