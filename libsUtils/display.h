#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <pico/types.h>
    // Diplay
    void displayInit(uint pin_in, uint pin_out);
    // void Display(const char *text, int x, int y, double scale, bool centerX, bool centerY);
    void Write(const char *text, int x, int scale, bool justified);
    void Display();
    // static void setup_gpios_leds(void);
    // void Leds(int red, int green, int blue);
#endif