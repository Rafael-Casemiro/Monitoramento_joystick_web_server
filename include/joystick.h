#ifndef JOYSTICK_INIT_H // Guardas de inclusão
#define JOYSTICK_INIT_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

#define VRX 26 // Pino de leitura do eixo X do joytick
#define VRY 27 // Pino de leitura do eixo Y do joystick
#define ADC_CHANNEL_0 0 // Canal ADC para o eixo X do joystick
#define ADC_CHANNEL_1 1 // Canal ADC para o eixo Y do joystick

// Função para configurar o joystick
void setup_joystick();

#endif