#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "ledsNeopixels.h"
#include <stdlib.h> // Para rand
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include <pico/stdlib.h> // Necessário para stdio_init_all e sleep_ms

#define PIN_DISPLAY_IN 14
#define PIN_DISPLAY_OUT 15

#define PIN_LEDS 7
#define NUM_LEDS 25
// Gabarito (Leds posições)
//      {24, 23, 22, 21, 20}};
//      {15, 16, 17, 18, 19},
//      {14, 13, 12, 11, 10},
//      {05, 06, 07, 08, 09},
//      {04, 03, 02, 01, 00},
int health[] = {2, 6, 14, 15, 23, 17, 21, 19, 10, 8}; // posicoes dos leds a serem acesos para formar o desenho
// abaixo as cores para o desenho, caso nao seja passado, serao usadas cores aleatorias
uint16_t colorHealth[10][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}};
// abaixo combinação de cores para o desenho
uint16_t colorHealthAlternates[][10][3] = {
    {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}},
    {{0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}},
    {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}},
    {{0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}},
    {{0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}},
    {{0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}},
    {{0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0255, 0}},
    {{0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}},
    {{0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}},
    {{0, 255, 0}, {0, 0, 255}, {0, 0255, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {0, 0255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}}};

int clock[12] = {21, 19, 10, 9, 1, 2, 3, 5, 14, 15, 23, 22};
uint16_t colorClock[12][12][3];

int main()
{
    stdio_init_all();
    char result[30];
    int sizeHealth = sizeof(health) / sizeof(health[0]);
    int sizeClock = sizeof(clock) / sizeof(clock[0]);
    int sizeColorClock = sizeof(colorClock) / sizeof(colorClock[0]);
    bool showDisplay = true;
    int counter = 0;
    NP_Init(NUM_LEDS, PIN_LEDS);
    Display_Init(PIN_DISPLAY_IN, PIN_DISPLAY_OUT);

    Display_Write("Jhacksonh", 0, 2, true);
    Display_Show();

    for (int r = 0; r < sizeClock; r++)
    {
        for (int c = 0; c < sizeClock; c++)
        {
            colorClock[r][c][0] = 0;
            colorClock[r][c][1] = 0;
            colorClock[r][c][2] = 2;
            if (c == r)
            {
                colorClock[r][c][0] = 100;
                colorClock[r][c][1] = 0;
                colorClock[r][c][2] = 0;
            }
        }
    }
    for (int i = 0; i < sizeof(colorHealthAlternates) / sizeof(colorHealthAlternates[0]); i++)
    {
        NP_DrawLeds(health, 10, 0, 230, colorHealthAlternates[i]); // comando desenha os Leds com as cores passadas, em cada posição
        sleep_ms(200);
    }
    NP_ResetLeds(); // comando para resetar os leds
    int ledCenter[1] = {12};
    uint16_t colorLedCenter[1][3] = {
        {0, 100, 0},
    };
    NP_DrawLeds(ledCenter, 1, 0, 0, colorLedCenter);
    sleep_ms(500);
    int counterMinites = 0;
    while (1)
    {
        // NP_DrawLeds(health, 10, 0, 275, NULL); //comando desenha os leds com cores aleatorias
        // NP_DrawLeds(health, sizeHealth, 0, 0, colorHealth); //comando desenha os Leds com as cores passadas, em cada posição
        // for para exibir as cores que foram passadas na combinação
        if (counter == 5)
        {
            counterMinites++;
            sprintf(result, "Min: %d", counterMinites);
            Display_Write("Jhacksonh", 0, 2, false);
            Display_Write(result, 0, 2, false);
            Display_Write(NP_GetStatus(), 0, 1, true);
            Display_Show();
            counter = 0;
            // showDisplay = false;
        }
        for (size_t i = 0; i < sizeClock; i++){
            NP_DrawLeds(clock, sizeClock, 0, 0, colorClock[i]); // comando desenha os leds com cores aleatorias
            sleep_ms(1000);
        }
        counter++;
        // sleep_ms(200);
    }
}
