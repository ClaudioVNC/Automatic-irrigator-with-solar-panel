# Irrigador Automático com ESP32 e Painel Solar

Este projeto é um sistema de irrigação automático, alimentado por um painel solar e controlado por um ESP32. O sistema usa sensores para monitorar umidade do solo, temperatura e inclinação, ativando a irrigação conforme a necessidade.

# Funcionalidades

- Monitoramento de umidade e temperatura com o sensor DHT11.
- Controle de irrigação com base no nível de umidade do solo.
- Envio de dados de telemetria via Wi-Fi para monitoramento remoto.
- Detecção de inclinação para garantir estabilidade do sistema.

# Como Funciona

O ESP32 lê os dados dos sensores de umidade, temperatura e inclinação. Caso a umidade esteja abaixo do limite, a válvula de irrigação é ativada. A inclinação é monitorada para garantir que o sistema permaneça em uma posição segura.

# Configuração e Instalação

Hardware necessário:
- ESP32
- Sensor de Umidade e Temperatura (DHT11)
- Acelerômetro (MPU6050)
- Relé 5V
- LEDs e resistores
- Painel solar (especificações a definir)
- Bateria para o ESP32 (especificações a definir)

Bibliotecas Arduino necessárias:
- DHT para o sensor DHT11
- Adafruit_MPU6050 para o acelerômetro
- Wire para comunicação I2C
- WiFi para a comunicação via Wi-Fi
- HTTPClient para o envio de dados ao servidor via HTTP
