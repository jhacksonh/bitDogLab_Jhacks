#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <pico/types.h>

    void Display_Init(uint pin_in, uint pin_out);
    void Display_Write(const char *text, int x, int scale,bool justified);
    void Display_Show();

#endif