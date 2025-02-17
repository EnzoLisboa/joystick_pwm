# Joystick com PWM

Este projeto utiliza um joystick para controlar a intensidade de LEDs RGB através de PWM (Modulação por Largura de Pulso) e exibe a posição do joystick em um display OLED SSD1306. O projeto foi desenvolvido para o Raspberry Pi Pico W, utilizando a placa de desenvolvimento BitDogLab.

## Funcionalidades

- **Controle de LEDs RGB**:
  - O LED Vermelho é controlado pelo eixo X do joystick.
  - O LED Azul é controlado pelo eixo Y do joystick.
  - O LED Verde é alternado ao pressionar o botão do joystick.
- **Exibição no Display**:
  - A posição do joystick é representada por um quadrado de 8x8 pixels no display OLED.
  - O estilo da borda do display é alternado ao pressionar o botão do joystick.
- **Controle de PWM**:
  - O PWM dos LEDs Vermelho e Azul pode ser ativado ou desativado ao pressionar o botão A.

## Estrutura do Código

O código está organizado da seguinte forma:

- **Inicialização**:
  - Configuração dos pinos do joystick, LEDs e botões.
  - Inicialização do display OLED SSD1306 via I2C.
  - Configuração do PWM para os LEDs RGB.
- **Loop Principal**:
  - Leitura dos valores analógicos do joystick (eixos X e Y).
  - Controle do brilho dos LEDs Vermelho e Azul com base nos valores do joystick.
  - Exibição da posição do joystick no display OLED.
  - Alternância do estilo da borda do display ao pressionar o botão do joystick.
  - Ativação/desativação do PWM ao pressionar o botão A.

## Requisitos

### Hardware

- **Raspberry Pi Pico W**.
- **Joystick** com dois eixos analógicos (X e Y) e um botão.
- **Display OLED SSD1306** (128x64 pixels) conectado via I2C.
- **LEDs RGB** conectados aos pinos GPIO.
- **Botão A** para controle adicional.
- **Placa de desenvolvimento BitDogLab** (ou equivalente).

### Software

- **SDK do Raspberry Pi Pico**.
- **Biblioteca SSD1306** para controle do display OLED.
- **Toolchain de compilação** para ARM (GCC).
- **CMake** para geração do projeto.

## Como Usar

### Compilação

1. Clone o repositório do projeto:
   ```bash
   git clone https://github.com/seu-usuario/joystick-pwm.git
   cd joystick-pwm

2. Crie e entre no diretório de build:
    ```bash
    mkdir build
    cd build

3. Gere os arquivos de build com CMake:
    ```bash
    cmake ..

4. Compile o projeto:
    ```bash
    make

### Upload

1. Conecte o Raspberry Pi Pico W ao computador via USB.

2. Coloque o Pico em modo de bootloader (segure o botão BOOTSEL enquanto conecta o USB).

3. Copie o arquivo .uf2 gerado para o Pico.

### Operação

1. Após o upload, o Pico iniciará o programa automaticamente.

2. Movimente o joystick para controlar os LEDs e o quadrado no display.

3. Pressione o botão do joystick para alternar o LED Verde e o estilo da borda do display.

4. Pressione o botão A para ativar/desativar o PWM dos LEDs Vermelho e Azul.

## Demonstração

[Assista ao vídeo da demonstração]()
