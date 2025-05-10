// Declaração de bibliotecas
#include <stdio.h>
#include "joystick.h"
#include "hardware/adc.h"

// Função para configurar o joystick
void setup_joystick() {
    adc_init(); // Inicializa o módulo ADC
    adc_gpio_init(VRX); // Configura o pino do eixo X para entrada ADC
    adc_gpio_init(VRY); // Configura o pino do eixo y para entrada ADC
}