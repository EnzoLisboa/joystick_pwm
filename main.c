#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/adc.h"


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



// Variáveis globais
bool led_green = false;
bool led_blue = true;
bool led_red = true;
static volatile uint32_t last_time = 0; // Define o tempo inicial (em microssegundos)



// Função para configurar os botões
void config_button(uint button)
{
    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);
}

// Função para configurar as leds

void config_led(uint led)
{
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);
    gpio_put(led, false);
}

// Função de interrupção
void gpio_irq_handler(uint gpio, uint32_t events) {

    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 250000) // 250 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        if (gpio == BUTTON_A_PIN) {
            led_red = !led_red;
            led_blue = !led_blue;
        }
        else if (gpio == BUTTON_JOYS_PIN) {
            led_green = !led_green;
            gpio_put(LED_GREEN_PIN, led_green);
        }
    }
}

int main()
{
    stdio_init_all();
    
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    ssd1306_t ssd; // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configura os botões
    config_button(BUTTON_A_PIN);
    config_button(BUTTON_JOYS_PIN);

    // Configura as LEDs

    config_led(LED_GREEN_PIN);
    config_led(LED_BLUE_PIN);
    config_led(LED_RED_PIN);

    // Configura as interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYS_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);


    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
}