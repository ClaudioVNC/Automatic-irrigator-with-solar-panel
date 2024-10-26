#define led_alim 2     // LED de alimentação no GPIO 2
#define led_irrig 25   // LED de irrigação no GPIO 25
#define led_wifi 26    // LED de Wi-Fi no GPIO 26
#define led_sens 27    // LED de Sensor no GPIO 27
#define rele 23        // Relé no GPIO 23
#define acele_SDA 21   // Acelerômetro (SDA) no GPIO 21
#define acele_SCL 22   // Acelerômetro (SCL) no GPIO 22
#define dht 4          // Sensor DHT11 no GPIO 4

#include <Wire.h> // Biblioteca para comunicação I2C
#include <Adafruit_MPU6050.h> // Biblioteca do acelerômetro
#include <DHT.h> // Biblioteca do DHT11
#include <WiFi.h> // Biblioteca WiFi
#include <HTTPClient.h> // Biblioteca conexão HTTP

DHT dhtSensor(dht, DHT11); // Inicializa o sensor DHT

Adafruit_MPU6050 mpu; // Inicializa o acelerômetro

const char* ssid = "SSID"; // Nome da rede Wi-Fi
const char* password = "PASSWORD"; // Senha da rede Wi-Fi

const char* servidorName = "http://servidor_qualquer.com/api"; // URL do servidor

unsigned long previousMillis = 0; // Armazena o tempo registrado

void setup()
{
  Serial.begin(115200); // Inicializa a comunicação serial
  Wire.begin(); // Inicializa a comunicação I2C
  WiFi.begin(ssid, password); // Inicializa o WiFi
  
  // Conectar rede
  Serial.println("Conectando ao WiFi...");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Conectado ao WiFi!");

  // Configura os LEDs como saída
  pinMode(led_alim, OUTPUT);
  pinMode(led_irrig, OUTPUT);
  pinMode(led_wifi, OUTPUT);
  pinMode(led_sens, OUTPUT);
  
  pinMode(rele, OUTPUT); // Configura o relé como saída
  
  dhtSensor.begin();  // Inicializa o DHT

  // Testa funcionalidade do acelerômetro
  if (!mpu.begin())
  {
    digitalWrite(led_alim, LOW);
    while (1) 
    {
      Serial.print("O acelerômetro falhou. Verifique a montagem!");
      delay(500);
    }
  }
}

void loop() 
{
  // Temporizador de 2 segundos
  if(millis() - previousMillis > 2000)
  {
    previousMillis = millis();

    // Lê a umidade e temperatura do DHT11
    float temperature = dhtSensor.readTemperature();
    float humidity = dhtSensor.readHumidity();
    
    if(temperature == 0 || humidity == 0) // Verifica se as leituras estão corretas
    {
      digitalWrite(led_sens, LOW);
    }
    else
    {
      digitalWrite(led_sens, HIGH);
    }
    
    sensors_event_t accelerometerData, t1, t2; // Variaveis para armazenar as informaçoes do DHT(t1 e t2 não sao usados em nenhum momento mas para usar a função "mpu.getEvent" preciso de 3 variaveis)
    mpu.getEvent(&accelerometerData, &t1, &t2); // Lê os dados do sensor

    float inclination = calcularInclinacao(accelerometerData.acceleration.y, accelerometerData.acceleration.z); // Calcular inclinação

    String jsonData = String("{\"Temperatura\":") + temperature + ", \"Umidade\":" + humidity + ", \"Inclinação\":" + inclination + "}"; // Informações do sistema

    Send_information(jsonData);
    
    // Lógica de controle do relé (ligar irrigação)
    if(humidity < 40.0 && inclination >= 45.0)
    {
      digitalWrite(rele, HIGH); 
      digitalWrite(led_irrig, HIGH); 
    } 
    else 
    {
      digitalWrite(rele, LOW);
      digitalWrite(led_irrig, LOW);
      if(inclination < 45.0)
      {
        Send_information("ALERTA!!!\nInclinação muito baixa!!!");
      }
    }

    // Colocar o ESP32 em Deep Sleep por 10 minutos
    esp_sleep_enable_timer_wakeup(600 * 1000000);
    Serial.println("Entrando em Deep Sleep por 2 minutos...");
    esp_deep_sleep_start();
  }
}

//Função para enviar as informações via WiFi
void Send_information(String data)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http; // Define http como tipo de HTTPClient

    http.begin(servidorName); // Inicializa o servidor

    http.addHeader("Content-Type", "application/json"); // Especifica o tipo de conteúdo para o envio de dados

    int httpResponseCode = http.POST(data); // Envia os dados com método POST

    if(httpResponseCode > 0) 
    {
      String answer = http.getString(); // Obter resposta do servidor, se houver
      Serial.println(answer); // Exibe a resposta
    } 
    else 
    {
      Serial.println("Erro na solicitação POST");
    }

    http.end(); // Fecha conexão
  }
  else
  {
    Serial.println("WiFi não conectado!");
  }
}
// Função para calcular a inclinação a partir dos dados do acelerômetro
float calcularInclinacao(float x, float z) 
{
  float angle = atan2(x, z) * 180.0 / PI; // Converte o valor para graus
  return abs(angle);
}
