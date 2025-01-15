#include <stdio.h>  // Para printf
#include <stdlib.h> // Para malloc
#include <pico/types.h>
#include <string.h>
#include <stdint.h>        // Para uint8_t
#include <hardware/sync.h> // Para sleep_us
#include <pico/time.h>
#include "ws2818b.pio.h"   // Programa PIO para LEDs
#include "ledsNeopixels.h" // Cabeçalho para funções de LEDs
#include <pico/stdlib.h>   // Necessário para stdio_init_all e sleep_ms

// Definição de pixel GRB
typedef struct{
    uint8_t G, R, B;
} npLED_t;
static npLED_t *LEDS_RGB = NULL;
static int LED_COUNT = 0;

// Variáveis para uso da máquina PIO.
static PIO np_pio;
static uint sm;

static char *status = "";

static void InitLEDS(){
    if (LED_COUNT <= 0) {
        status = "Erro: Contagem de LEDs inválida (menor ou igual a 0).";
        return;
    }
    LEDS_RGB = (npLED_t *)malloc(sizeof(npLED_t) * LED_COUNT);
    if (LEDS_RGB == NULL){
        status = "Erro: Falha ao alocar memoria para LEDs.";
        return;
    }
    for (uint i = 0; i < LED_COUNT; ++i){
        LEDS_RGB[i].R = 0;
        LEDS_RGB[i].G = 0;
        LEDS_RGB[i].B = 0;
    }
}

static uint8_t *randIntRGB(int min,int max){
    if (min > max) {
        status = "Erro: Intervalo inválido para geração de cor RGB.(min,max)";
        return NULL;
    }
    uint8_t *rgb = malloc(3 * sizeof(int)); // Aloca espaço para RGB
    if (!rgb){
        status = "Erro ao alocar memoria para RGB";
        return NULL;
    }
    rgb[0] = rand() % (max - min+1) + min;
    rgb[1] = rand() % (max - min+1) + min;
    rgb[2] = rand() % (max - min+1) + min;
    return rgb;
}

static void FreeRGB(){
    if (LEDS_RGB != NULL)
    {
        free(LEDS_RGB);
        LEDS_RGB = NULL;
    }
}
// Função para definir a cor de um LED
static void WriteLeds(){
    if (LEDS_RGB == NULL){
        status = "Erro: LEDs nao inicializados";
        return;
    }
    for (int i = 0; i < LED_COUNT; i++){
        pio_sm_put_blocking(np_pio, sm, LEDS_RGB[i].G);
        pio_sm_put_blocking(np_pio, sm, LEDS_RGB[i].R);
        pio_sm_put_blocking(np_pio, sm, LEDS_RGB[i].B);
    }
}

// Função de inicialização
void NP_Init(uint led_count, uint pin){
    if (led_count <= 0) {
        status = "Erro: Contagem de LEDs deve ser maior que 0.";
        return;
    }
    LED_COUNT = led_count;
    uint offset = pio_add_program(pio0, &ws2818b_program);
    if (offset == (uint)-1) {
        status = "Erro: Não foi possível adicionar o programa PIO.";
        return;
    }
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0){
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true);
    }
    if (sm < 0) {
        status = "Erro: Não foi possível alocar uma máquina de estado PIO.";
        return;
    }
    ws2818b_program_init(np_pio, sm, offset, pin, 800000); // Inicializa o programa WS2818b
    InitLEDS();
}

void NP_DrawLeds(int *drawPosLeds, size_t sizePosDrawOn, int min, int max, uint16_t (*RGB_Colors)[3], uint32_t time) {
    if (sizePosDrawOn <= 0 || drawPosLeds == NULL) {
        status = "Erro: Posições de LEDs inválidas.";
        return;
    }

    // Ajustando min e max com segurança
    max = max > 255 ? 255 : (max < 0 ? 0 : max);
    min = min > 255 ? 255 : (min < 0 ? 0 : min);

    int erro[11] = {21, 22, 23, 16, 13, 12, 11, 6, 3, 2, 1};

    // Quando RGB_Colors é NULL e max é 0
    if (RGB_Colors == NULL && max == 0) {
        sizePosDrawOn = sizeof(erro) / sizeof(erro[0]);
        // Não fazer 'free' diretamente de drawPosLeds sem garantir sua alocação.
        // Em vez disso, alocar uma nova memória e manter a original intacta
        int *newDrawPosLeds = malloc(sizePosDrawOn * sizeof(int));
        if (newDrawPosLeds == NULL) {
            status = "Erro: Falha ao alocar memória para drawPosLeds.";
            return;
        }

        memcpy(newDrawPosLeds, erro, sizePosDrawOn * sizeof(int)); // Copiar o conteúdo do erro
        drawPosLeds = newDrawPosLeds; // Atualizar drawPosLeds com a nova alocação

        RGB_Colors = malloc(sizePosDrawOn * sizeof(int[3]));
        if (RGB_Colors == NULL) {
            status = "Erro: Falha ao alocar memoria para RGB_Colors.";
            free(drawPosLeds);  // Liberar memória se falhar
            return;
        }

        // Inicializar RGB_Colors com valores padrão
        for (size_t i = 0; i < sizePosDrawOn; i++) {
            RGB_Colors[i][0] = 2;
            RGB_Colors[i][1] = 0;
            RGB_Colors[i][2] = 0;
        }
        status = "ERRO: voce nao informou as cores para os leds.";
    }

    for (size_t i = 0; i < 25; i++){
        int isLedOn = 0;
        for (size_t j = 0; j < sizePosDrawOn; j++){
            if ((size_t)i == drawPosLeds[j]){ // LED deve estar aceso
                uint8_t *rgb = randIntRGB(min, max);
                if (rgb==NULL) return; // Se falhou na alocação de RGB
                LEDS_RGB[i].R = RGB_Colors == NULL ? rgb[0] : RGB_Colors[j][0];
                LEDS_RGB[i].G = RGB_Colors == NULL ? rgb[1] : RGB_Colors[j][1];
                LEDS_RGB[i].B = RGB_Colors == NULL ? rgb[2] : RGB_Colors[j][2];
                isLedOn = 1;
                free(rgb); // Liberar memória alocada para rgb
                break;
            }
        }
        if (!isLedOn){ // LED apagado
            LEDS_RGB[i].R = 0;
            LEDS_RGB[i].G = 0;
            LEDS_RGB[i].B = 0;
        }
    }
    WriteLeds();
}

char *NP_GetStatus(){
    return status;
}

// Função para resetar todos os LEDs
void NP_ResetLeds(){
    FreeRGB();
    InitLEDS();
}
