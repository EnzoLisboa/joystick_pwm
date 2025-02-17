#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "inc/ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Pinos dos LEDs RGB
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13

// Pinos dos botões
#define BUTTON_A_PIN 5
#define BUTTON_JOYS_PIN 22

// Pinos dos potenciômetros do joystick
#define VRX_PIN 26
#define VRY_PIN 27

// Variáveis globais
bool led_green = false; // Estado do LED Verde
bool pwm_enabled = true; // Estado do PWM (ativo ou desativado)
static volatile uint32_t last_time = 0; // Tempo do último evento (para debouncing)
uint8_t border_style = 0; // Estilo da borda do display (0 = sem borda, 1 = borda simples, 2 = borda dupla)

// Função para configurar os botões
void config_button(uint button)
{
    gpio_init(button); // Inicializa o pino do botão
    gpio_set_dir(button, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button); // Habilita o resistor de pull-up
}

// Função para configurar os LEDs
void config_led(uint led)
{
    gpio_init(led); // Inicializa o pino do LED
    gpio_set_dir(led, GPIO_OUT); // Configura o pino como saída
    gpio_put(led, false); // Inicialmente, o LED está apagado
}

// Função para configurar o PWM nos LEDs
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino para função PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o slice do PWM
    pwm_set_wrap(slice_num, 4095); // Define o valor máximo do PWM (12 bits)
    pwm_set_enabled(slice_num, true); // Habilita o PWM
}

// Função de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento (debouncing)
    if (current_time - last_time > 250000) // 250 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        if (gpio == BUTTON_A_PIN) {
            pwm_enabled = !pwm_enabled; // Ativa ou desativa o PWM
        }
        else if (gpio == BUTTON_JOYS_PIN) {
            led_green = !led_green; // Alterna o estado do LED Verde
            gpio_put(LED_GREEN_PIN, led_green); // Atualiza o estado do LED Verde
            border_style = (border_style + 1) % 3; // Alterna o estilo da borda
        }
    }
}

// Função para mapear o valor do joystick para o brilho do LED
uint16_t map_joystick_to_led(uint16_t joystick_value) {
    // Valor central do joystick (2048)
    const uint16_t center_value = 2048;
    // Valor máximo do joystick (4095)
    const uint16_t max_value = 4095;

    // Calcula a diferença em relação ao centro
    int16_t diff = (int16_t)joystick_value - center_value;

    // Se o joystick estiver próximo ao centro, retorna 0 (LED apagado)
    if (diff > -50 && diff < 50) {
        return 0;
    }

    // Mapeia o valor do joystick para o intervalo de 0 a 4095
    if (diff < 0) {
        return (center_value - joystick_value) * 2; // Movimento para cima ou para a esquerda
    } else {
        return (joystick_value - center_value) * 2; // Movimento para baixo ou para a direita
    }
}

int main()
{
    stdio_init_all(); // Inicializa a comunicação serial (para debug)

    // Inicializa o ADC e os pinos dos potenciômetros do joystick
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    
    // Inicialização do I2C a 400 kHz
    i2c_init(I2C_PORT, 400 * 1000);

    // Configura os pinos do I2C
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA para função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL para função I2C
    gpio_pull_up(I2C_SDA); // Habilita o resistor de pull-up no pino SDA
    gpio_pull_up(I2C_SCL); // Habilita o resistor de pull-up no pino SCL

    // Inicialização do display SSD1306
    ssd1306_t ssd; // Estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display (todos os pixels apagados)
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configura os botões
    config_button(BUTTON_A_PIN);
    config_button(BUTTON_JOYS_PIN);

    // Configura os LEDs
    config_led(LED_GREEN_PIN);
    config_led(LED_BLUE_PIN);
    config_led(LED_RED_PIN);

    // Configura o PWM para os LEDs RGB
    setup_pwm(LED_RED_PIN);
    setup_pwm(LED_BLUE_PIN);

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYS_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop principal
    while (true)
    {
        // Leitura dos valores do joystick (eixos X e Y)
        adc_select_input(0); // Seleciona o eixo Y (VRY)
        uint16_t vry_value = adc_read(); // Lê o valor do eixo Y
        adc_select_input(1); // Seleciona o eixo X (VRX)
        uint16_t vrx_value = adc_read(); // Lê o valor do eixo X

        // Inverte o eixo Y para corrigir o movimento no display
        vry_value = 4095 - vry_value;

        // Mapeia os valores do joystick para o brilho dos LEDs
        uint16_t led_red_brightness = map_joystick_to_led(vrx_value); // Brilho do LED Vermelho (eixo X)
        uint16_t led_blue_brightness = map_joystick_to_led(vry_value); // Brilho do LED Azul (eixo Y)

        // Controle do LED Vermelho (eixo X)
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, led_red_brightness); // Ajusta o brilho do LED Vermelho
        } else {
            pwm_set_gpio_level(LED_RED_PIN, 0); // Desliga o LED Vermelho
        }

        // Controle do LED Azul (eixo Y)
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_BLUE_PIN, led_blue_brightness); // Ajusta o brilho do LED Azul
        } else {
            pwm_set_gpio_level(LED_BLUE_PIN, 0); // Desliga o LED Azul
        }

        // Exibição da posição do joystick no display
        ssd1306_fill(&ssd, false); // Limpa o display
        uint8_t x_pos = (vrx_value * 120) / 4095; // Mapeia o valor do eixo X para a largura do display
        uint8_t y_pos = (vry_value * 56) / 4095; // Mapeia o valor do eixo Y para a altura do display
        ssd1306_rect(&ssd, y_pos, x_pos, 8, 8, true, true); // Desenha um quadrado de 8x8 pixels

        // Desenha a borda do display de acordo com o estilo atual
        switch (border_style) {
            case 1:
                ssd1306_rect(&ssd, 0, 0, 128, 64, true, false); // Borda simples
                break;
            case 2:
                ssd1306_rect(&ssd, 0, 0, 128, 64, true, false); // Borda dupla (externa)
                ssd1306_rect(&ssd, 2, 2, 124, 60, true, false); // Borda dupla (interna)
                break;
            default:
                // Sem borda (border_style == 0)
                break;
        }

        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(50); // Pequeno delay para evitar flickering
    }
}