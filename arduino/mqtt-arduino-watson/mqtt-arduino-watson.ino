#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>

//Inclusão de biblioteca para leitura do sensor DHT11
#include "DHT.h"

// Sensor de temperatura e umidade do ambiente 
#define DHTPIN A1// pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados) 
// e ao pino 1 (VCC) do sensor
DHT dht(DHTPIN, DHTTYPE);

// Sensor de umidade do solo
#define pino_sinal_analogico A0

// variável para receber valor do sensor de umidade do solo
int valor_analogico;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
// IPAddress ip(192, 168, 1, 61);
IPAddress server(169, 62, 202, 130);

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);


void setup()
{
    // Open serial communications and wait for port to open:

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // Setup dos sensores
  pinMode(pino_sinal_analogico, INPUT);
  dht.begin();
  
  // Ethernet.begin(mac, ip);
  Ethernet.begin(mac);
  // Allow the hardware to sort itself out
  delay(1500);

  mqttClient.setServer(server, 1883);

  if (mqttClient.connect("d:mc6re7:arduino:a2560", "use-token-auth", "12345678")) {
    // connection succeeded
    Serial.println("Connected ");
    // boolean r= mqttClient.subscribe("test");
    // Serial.println("subscribe ");
    // Serial.println(r);
  } 
  else {
    // connection failed
    // mqttClient.state() will provide more information
    // on why it failed.
    Serial.println("Connection failed ");
  }

}

  void loop()
  {
  
  while (true)
  {

  // Leitura sensores
  // A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade do ambiente: ");
    Serial.print(h);
    Serial.print("% - ");
    Serial.print("Temperatura do ambiente: ");
    Serial.print(t);
    Serial.println(" *C");
  }
  // Leitura da umidade do solo
  // Lê o valor do pino A0 do sensor
  valor_analogico = analogRead(pino_sinal_analogico);
  //Mostra o valor da porta analogica no serial monitor
  Serial.print("Porta analogica: ");
  Serial.print(valor_analogico);

  //Solo umido
  if (valor_analogico >= 452)
  {
    Serial.println(" Status: Solo umido");
  }
 
  //Solo com umidade moderada
  if (valor_analogico < 452 && valor_analogico >= 226)
  {
    Serial.println(" Status: Umidade moderada");
  }
 
  //Solo seco
  if (valor_analogico < 226)
  {
    Serial.println(" Status: Solo seco");
  }
  delay(1000);
  
  // Comunicação MQTT
  Serial.println("publishing string ");
  
  boolean rc = mqttClient.publish("iot-2/evt/telemetria/fmt/json","{\"temperatura ambiente\":\"20\"}");
  Serial.println("publishing Temp");
   //rc = mqttClient.publish("testbyte", outmsg,2);
    delay(1000); //wait
    mqttClient.loop(); //call loop
  }
    
  }
