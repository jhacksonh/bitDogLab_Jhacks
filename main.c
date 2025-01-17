#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "display.h"
#include "ledsNeopixels.h"
#include <stdlib.h>      // Para rand
#include <pico/stdlib.h> // Necessário para stdio_init_all e sleep_ms
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"

#define PIN_DISPLAY_IN 14
#define PIN_DISPLAY_OUT 15

#define PIN_BUTTON_A 5
#define PIN_BUTTON_B 6
#define PIN_BUTTON_JOY 22

#define PIN_BUZZER_A 21
#define PIN_BUZZER_B 10

#define PIN_LEDS 7
#define NUM_LEDS 25

int freq = 5;
int timeBuzzer = 500;
bool modTimeBuzzer_Active = false;
char status[100] = "";
char status_Actual[100] = "";

// Gabarito (Leds posições)
//      {24, 23, 22, 21, 20}};
//      {15, 16, 17, 18, 19},
//      {14, 13, 12, 11, 10},
//      {05, 06, 07, 08, 09},
//      {04, 03, 02, 01, 00},
int health[] = {2, 6, 14, 15, 23, 17, 21, 19, 10, 8}; // posicoes dos leds a serem acesos para formar o desenho
int sizeHealth = sizeof(health) / sizeof(health[0]);
// abaixo as cores para o desenho, caso nao seja passado, serao usadas cores aleatorias
uint16_t colorHealth[10][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0}};
int sizeColorHealth = sizeof(colorHealth) / sizeof(colorHealth[0]);
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
int sizeColorHealthAlternates = sizeof(colorHealthAlternates) / sizeof(colorHealthAlternates[0]);

int clockD[12] = {21, 19, 10, 9, 1, 2, 3, 5, 14, 15, 23, 22};
int sizeClock = sizeof(clockD) / sizeof(clockD[0]);
uint16_t colorClock[12][12][3];
int sizeColorClock = sizeof(colorClock) / sizeof(colorClock[0]);
int ledCenter[1] = {12};
uint16_t colorLedCenter[1][3] = {{0, 100, 0}};

void Init_Gpios(uint Gpio, bool PinIN, uint pull_up)
{
    gpio_init(Gpio);
    gpio_set_dir(Gpio, PinIN ? GPIO_IN : GPIO_OUT);
    if (pull_up)
    {
        gpio_pull_up(Gpio);
    }
}
void Show_in_Display()
{
    char freqChar[30] = "";
    char timeChar[30] = "";
    char result[30] = "";

    sprintf(freqChar, "Freq: %d", freq);
    sprintf(timeChar, "Tempo: %d", timeBuzzer);
    sprintf(result, "Reuzir_Tempo: %s", modTimeBuzzer_Active ? "Sim" : "Nao");

    Display_Write("Jhacksonh", 0, 2, false);
    Display_Write(freqChar, 0, 1, false);
    Display_Write(timeChar, 0, 1, false);
    Display_Write(result, 0, 1, false);
    Display_Show();
}
void CreateDrawHealth()
{
    for (int i = 0; i < sizeColorHealthAlternates; i++)
    {
        NP_DrawLeds(health, 10, 0, 230, colorHealthAlternates[i]); // comando desenha os Leds com as cores passadas, em cada posição
        sleep_ms(200);
    }
    NP_ResetLeds(); // comando para resetar os leds
}
void CreateDrawClock()
{
    for (size_t i = 0; i < sizeClock; i++)
    {
        sleep_ms(1000);
        NP_DrawLeds(clockD, sizeClock, 0, 0, colorClock[i]); // comando desenha os leds com cores aleatorias
        sprintf(status, "F:%d T:%d %s", freq, timeBuzzer, modTimeBuzzer_Active ? "Ativado" : "Desativado");
        printf("Status: %s. \nModificado: %s\n", status, status_Actual == status ? "Nao" : "Sim");
        if (strcmp(status, status_Actual) != 0)
        {
            printf("Entramos\n");
            sprintf(status_Actual, "%s", status);
            Show_in_Display();
        }
    }
}
void button_callback(uint gpio, uint32_t events)
{
    static uint32_t last_press_time = 0;                  // Armazena o tempo da última pressão do botão
    uint32_t now = to_ms_since_boot(get_absolute_time()); // Tempo atual em ms

    if (now - last_press_time < 200)
        return; // Ignora se está dentro do intervalo de 100ms
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        if (gpio == PIN_BUTTON_A)
        {
            if (freq > 1 && !modTimeBuzzer_Active)
            {
                freq--;
            }
            else if (modTimeBuzzer_Active && timeBuzzer > 1)
            {
                timeBuzzer -= 10;
            }
        }
        else if (gpio == PIN_BUTTON_B)
        {
            if (!modTimeBuzzer_Active)
            {
                freq++;
            }
            else if (modTimeBuzzer_Active)
            {
                timeBuzzer += 10;
            }
        }
        else if (gpio == PIN_BUTTON_JOY)
        {
            modTimeBuzzer_Active = !modTimeBuzzer_Active;
        }
    }
}
void play_tone(uint buzzer_pin, int frequency, int duration_ms)
{
    // Calcula a frequência para o PWM
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin); // Obter o "slice" de PWM
    uint32_t clock = 125000000;                         // Clock padrão do Pico (125 MHz)
    uint32_t divider = clock / (frequency * 4096);      // Frequência * resolução do PWM
    if (divider < 1)
        divider = 1; // Divisão mínima
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 4095); // Resolução de 12 bits
    // Define o ciclo de trabalho (50% para um tom claro)
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(buzzer_pin), 2048);
    pwm_set_enabled(slice_num, true); // Ativar PWM
    // Toca o som pelo tempo especificado
    sleep_ms(duration_ms);
    // Desativa o PWM
    pwm_set_enabled(slice_num, false);
}
int main()
{
    stdio_init_all();
    bool showDisplay = true;

    int counter = 0;
    int counterMinites = 0;

    NP_Init(NUM_LEDS, PIN_LEDS);
    Display_Init(PIN_DISPLAY_IN, PIN_DISPLAY_OUT);
    Init_Gpios(PIN_BUTTON_A, true, true);
    Init_Gpios(PIN_BUTTON_B, true, true);
    Init_Gpios(PIN_BUTTON_JOY, true, true);

    gpio_set_function(PIN_BUZZER_A, GPIO_FUNC_PWM); // Inicializa o pino do buzzer como saída PWM
    gpio_set_function(PIN_BUZZER_B, GPIO_FUNC_PWM); // Inicializa o pino do buzzer como saída PWM
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_JOY, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    // Display_Write("Jhacksonh", 0, 2, true);
    // Display_Show();

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
    CreateDrawHealth();
    Show_in_Display();

    NP_DrawLeds(ledCenter, 1, 0, 0, colorLedCenter);
    sleep_ms(500);

    while (1)
    {
        // NP_DrawLeds(health, 10, 0, 275, NULL); //comando desenha os leds com cores aleatorias
        // NP_DrawLeds(health, sizeHealth, 0, 0, colorHealth); //comando desenha os Leds com as cores passadas, em cada posição
        CreateDrawClock();
        play_tone(PIN_BUZZER_A, freq, timeBuzzer);     // Toca (dinamicamente) Hz (nota A) por (dinâmico) ms
        play_tone(PIN_BUZZER_B, freq + 1, timeBuzzer); // Toca (dinamicamente) Hz por (dinâmico) ms
    }
}