#ifndef LEDSNEOPIXELS_H
#define LEDSNEOPIXELS_H
#include <stdint.h>         // Para uint8_t
#include <pico/types.h>

// Leds_neopixels
void NP_Init(uint led_count, uint pin);
void NP_DrawLeds(int* drawPosLeds, size_t sizePosDrawOn, int min, int max,uint16_t (*RGB_Colors)[3]);
char *NP_GetStatus();
void NP_ResetLeds();
#endif
