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
#define led_pin_r 12

#define PIN_LEDS 7
#define NUM_LEDS 25
//Gabarito (Leds posições) 
//     {24, 23, 22, 21, 20}};
//     {15, 16, 17, 18, 19},
//     {14, 13, 12, 11, 10},
//     {05, 06, 07, 08, 09},
//     {04, 03, 02, 01, 00},
int health[10] = {2, 6, 14, 15, 23, 17, 21, 19, 10, 8};//posicoes dos leds a serem acesos para formar o desenho
// abaixo as cores para o desenho, caso nao seja passado, serao usadas cores aleatorias	
uint16_t colorHealth[10][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}};

int main()
{
    stdio_init_all();
    bool asc = 1;
    int ledC = 0;
    char result[30];

    displayInit(PIN_DISPLAY_IN, PIN_DISPLAY_OUT);
    NP_Init(NUM_LEDS, PIN_LEDS);
    // Write("Funciona",0,2,false);
    // Display();
    bool showDisplay = true;
while (1){
    NP_DrawLeds(health, 10, 0, 275, NULL,300); //comando desenha os leds com cores aleatorias
    // NP_DrawLeds(health, 10, 0, 0, colorHealth,300); //comando desenha os Leds com as cores passadas, em cada posição
    // NP_ResetLeds(); //comando para resetar os leds
    if(showDisplay){
        Write("Jhacsonh", 0, 2, false);
        Write(NP_GetStatus(), 0, 1, false);
        Display();
        showDisplay = false;
    }
    sleep_ms(500);
}
}
