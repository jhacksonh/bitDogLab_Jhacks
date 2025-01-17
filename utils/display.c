#include "display.h"
#include <stdint.h>
#include "stdio.h"

#include "pico/types.h"
#include "ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>
#include <ctype.h>
#include <math.h>

#define SLEEPTIME 100 // Tempo de espera em microsegundos entre as animações
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8
#define MIN_SCALE 1.0 // Escala mínima permitida

// Estrutura para armazenar textos acumulados
typedef struct {
    char text[128];  // Texto armazenado
    int x, y;        // Posição do texto
    double scale;    // Escala do texto
} TextElement;

// Estrutura para gerenciar todos os textos
typedef struct {
    TextElement elements[20]; // Máximo de 20 textos
    int count;                // Número de textos armazenados
} TextAccumulator;

static TextAccumulator textAccumulator = {.count = 0};
static TextElement *prev;

// Função para inicializar o I2C
void Display_Init(uint pin_in, uint pin_out) {
    i2c_init(i2c1, 400000); // Inicializa o I2C com a frequência de 400 kHz
    gpio_set_function(pin_in, GPIO_FUNC_I2C);
    gpio_set_function(pin_out, GPIO_FUNC_I2C);
    gpio_pull_up(pin_in);
    gpio_pull_up(pin_out);
}
// Função para limpar o display
static void Clear(ssd1306_t *display) {
    ssd1306_clear(display);
    sleep_us(50); // Tempo de espera após limpar o display                                                                                              
}
// Função para inicializar o display
static void Render(ssd1306_t *display) {
    display->external_vcc = false;
    ssd1306_init(display, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0x3C, i2c1);
    Clear(display);
}
// Função para desenhar o texto no display
static void draw_text(ssd1306_t *display, int x, int y, int scale, const char *buffer) {
    ssd1306_draw_string(display, x, y, scale, buffer);
    ssd1306_show(display);
}
// Função para converter texto para maiúsculas
static char* to_uppercase(const char *text) {
    static char uppercased[128];
    int i = 0;
    while (text[i] != '\0' && i < sizeof(uppercased) - 1) {
        uppercased[i] = toupper((unsigned char)text[i]);
        i++;
    }
    uppercased[i] = '\0';
    return uppercased;
}
// Função auxiliar para processar quebra de linha
static void process_line_break(TextElement *element, int *currentLineLength, int *lastSpaceIndex, int *lastSpaceDestIndex, int *destIndex, const char *text, int maxWidth, double charWidth, int *i) {
    if (*currentLineLength >= maxWidth) {
        if (*lastSpaceIndex != -1) {
            element->text[*lastSpaceDestIndex] = '\0';
            *i = *lastSpaceIndex;
        } else {
            element->text[*destIndex] = '\0';
        }
    }
}
void Display_Write(const char *text, int x, int scale,bool justified) {
    // Verificar se o buffer de saída do display está cheio
    if (textAccumulator.count >= sizeof(textAccumulator.elements) / sizeof(TextElement)) {
        return;
    }
    // Calcular largura e altura do texto
    int maxWidth = DISPLAY_WIDTH;
    int textHeight = round(CHAR_HEIGHT * scale);
    int charWidth = round((CHAR_WIDTH + 1) * scale);

    // Variáveis auxiliares para controlar a quebra de linha
    int currentLineLength = 0;
    int destIndex = 0;
    int lastSpaceIndex = -1;
    int lastSpaceDestIndex = -1;

    // Alocar elemento no buffer de saída do display
    TextElement *element = &textAccumulator.elements[textAccumulator.count];
    element->x = x;
    element->scale = (scale < MIN_SCALE) ? MIN_SCALE : scale;
    // Calcular a coordenada y do texto
    if (textAccumulator.count == 0) {
        element->y = 0;
    } else {
        prev = &textAccumulator.elements[textAccumulator.count - 1];
        // int scalePrev = prev->scale == 2 ? 2 : (5*round(prev->scale/2))-prev->scale;
        element->y = prev->y + CHAR_HEIGHT*prev->scale + 1;
    }
    // Percorrer o texto e adicionar ao buffer de saída do display
    for (int i = 0; text[i] != '\0'; i++) {
        currentLineLength += charWidth;
        // Verificar se há espaço em branco e armazenar a posição do último espaço
        if (text[i] == ' ') {
            lastSpaceIndex = i;
            lastSpaceDestIndex = destIndex;
        }

        // Verificar se a linha atual está muito longa
        if (currentLineLength >= maxWidth) {
            // Processar quebra de linha
            process_line_break(element, &currentLineLength, &lastSpaceIndex, &lastSpaceDestIndex, &destIndex, text, maxWidth, charWidth, &i);
            // Incrementar contagem de elementos do buffer de saída do display
            textAccumulator.count++;
            // Verificar se o buffer de saída do display está cheio
            if (textAccumulator.count >= sizeof(textAccumulator.elements) / sizeof(TextElement)) {
                return;
            }
            // Alocar novo elemento do buffer de saída do display
            element = &textAccumulator.elements[textAccumulator.count];
            element->x = x;
            if(!justified)element->x = 0;
            element->scale = scale;
            element->y = textAccumulator.elements[textAccumulator.count - 1].y+round(CHAR_HEIGHT * scale);
            destIndex = 0;
            currentLineLength = 0;
            lastSpaceIndex = -1;
            lastSpaceDestIndex = -1;
            continue;
        }
        // Adicionar caractere ao buffer de saída do display
        if (destIndex < sizeof(element->text) - 1) {
            element->text[destIndex++] = text[i];
        }
    }
    // Adicionar caractere de fim de string ao buffer de saída do display
    element->text[destIndex] = '\0';

    // Incrementar contagem de elementos do buffer de saída do display
    textAccumulator.count++;
}
void Display_Show() {
    ssd1306_t display;
    Render(&display);
    for (int i = 0; i < textAccumulator.count; i++) {
        TextElement *element = &textAccumulator.elements[i];
        char *text = to_uppercase(element->text);
        draw_text(&display, element->x, element->y, element->scale, text);
    }
    memset(&textAccumulator, 0, sizeof(textAccumulator));   // Limpa o buffer de saída do display  
    // memset(&textAccumulator, 0, sizeof(textAccumulator));   // Limpa o buffer de saída do display                                                                                                
}
