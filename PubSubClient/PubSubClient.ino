#include <Ethernet.h>
// #include <ESP8266WiFi.h>
#include <PubSubClient.h>
// #include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// #define MQTT_MAX_PACKET_SIZE 256

// wifi credentials
// const char* wifi_ssid = "";
// const char* wifi_password = "";

//Inclusão de biblioteca para leitura do sensor DHT11
#include "DHT.h"

// Sensor de temperatura e umidade do ambiente
#define DHTPIN A1     // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados)
// e ao pino 1 (VCC) do sensor
DHT dht(DHTPIN, DHTTYPE);

// Sensor de umidade do solo
#define pino_sinal_analogico A0

#define MUITO_UMIDO 1
#define UMIDO 4
#define UMIDADE_MODERADA 3
#define SECO 2
#define MUITO_SECO 0


// variáveis que quardaram os valores dos sensores
int umidade_ambiente;
int temperatura_ambiente;
int valor_analogico_sensor_umidade_solo;
int umidade_solo;
int status_solo;

// variável para montar o JSON
// char* data;
// StaticJsonBuffer<256> jsonBuffer;
// String deviceEvent;
// String monta_json;

IPAddress server(192, 168, 1, 200);
// HostName=arduino-mega.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=nqBxfcg1PsDKOXHvhERcZFav/W9pZoDqITmgGDcRN8A=
// example: <myiothub>.azure-devices.net
// const char *iothub_url = "arduino-mega.azure-devices.net";

// this is the id of the device created in Iot Hub
// example: myCoolDevice
const char *iothub_deviceid = "amega2560";

// <myiothub>.azure-devices.net/<myCoolDevice>
const char *iothub_user = "arduino-mega.azure-devices.net/amega2560";

// SAS token should look like "SharedAccessSignature sr=<myiothub>.azure-devices.net%2Fdevices%2F<myCoolDevice>&sig=123&se=456"
const char *iothub_sas_token = "SharedAccessSignature sr=arduino-mega.azure-devices.net%2fdevices%2famega2560&sig=FSf8g66U+pSx4W3EjG9tx8h4Xw3dvQOSfPmSfKWUPyE=&se=1620442373";

// default topic feed for subscribing is "devices/<myCoolDevice>/messages/devicebound/#""
const char *iothub_subscribe_endpoint = "devices/amega2560/messages/devicebound/#";

// default topic feed for publishing is "devices/<myCoolDevice>/messages/events/"
const char *iothub_publish_endpoint = "devices/amega2560/messages/events/";

byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
// WiFiClientSecure espClient;
EthernetClient ethClient;
PubSubClient client(ethClient);

long lastMsg = 0;

// function to connect to the wifi
// void setup_wifi() {
//   delay(10);

//   Serial.println();
//   Serial.print("Connecting to wifi");

//   WiFi.begin(wifi_ssid, wifi_password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

// debug wifi via serial
//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// function to connect to MQTT server
void connect_mqtt()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // if (client.connect(iothub_deviceid, iothub_user, iothub_sas_token))
    if (client.connect(iothub_deviceid))
    {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(iothub_subscribe_endpoint);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// callback function for when a message is dequeued from the MQTT server
void callback(char *topic, byte *payload, unsigned int length)
{
  // print message to serial for debugging
  Serial.println("Message arrived: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  digitalWrite(7, HIGH);
  delay(7000); // Wait for 1000 millisecond(s)
  digitalWrite(7, LOW);
  // Serial.println('---');
}

void setup()
{
  // begin serial for debugging purposes
  Serial.begin(9600);
  Ethernet.begin(mac);
  delay(2500);
  // connect to wifi
  // setup_wifi();
  pinMode(7, OUTPUT);
  pinMode(pino_sinal_analogico, INPUT);
  dht.begin();
  // set up connection and callback for MQTT server
  // client.setServer(iothub_url, 8883);
  client.setServer(server, 1883);
  client.setCallback(callback);
  // client.setBufferSize(1024);

  // connect to MQTT
  connect_mqtt();
}

void loop()
{
  client.loop();
  long now = millis();

  // publish data and debug mqtt connection every 10 seconds
  if (now - lastMsg > 10000)
  {

    // Leitura sensores
    // A leitura da temperatura e umidade pode levar 250ms!
    // O atraso do sensor pode chegar a 2 segundos.
    umidade_ambiente = dht.readHumidity();
    temperatura_ambiente = dht.readTemperature();
    // testa se retorno é valido, caso contrário algo está errado.
    if (isnan(temperatura_ambiente) || isnan(umidade_ambiente))
    {
      Serial.println("Falha na leitura do sensor DHT");
    }
    else
    {
      Serial.println("Umidade do ambiente: ");
      Serial.print(umidade_ambiente);
      Serial.print(" % - ");
      Serial.print("Temperatura do ambiente: ");
      Serial.print(temperatura_ambiente);
      Serial.println(" ºC");
    }
    // Leitura da umidade do solo
    // Lê o valor do pino A0 do sensor
    valor_analogico_sensor_umidade_solo = analogRead(pino_sinal_analogico);
    //Mostra o valor da porta analogica no serial monitor
    Serial.print("Valor capturado da porta analógica: ");
    Serial.print(valor_analogico_sensor_umidade_solo);

    //Solo muito úmido
    if (valor_analogico_sensor_umidade_solo >= 504)
    {
      status_solo = MUITO_UMIDO;
      Serial.println(" - Status do solo: " + (String)status_solo);
    }

    //Solo úmido
    if (valor_analogico_sensor_umidade_solo < 504 && valor_analogico_sensor_umidade_solo >= 378)
    {
      status_solo = UMIDO;
      Serial.println(" - Status do solo: " + (String)status_solo);
    }

    //Solo com umidade moderada
    if (valor_analogico_sensor_umidade_solo < 378 && valor_analogico_sensor_umidade_solo >= 252)
    {
      status_solo = UMIDADE_MODERADA;
      Serial.println(" - Status do solo: " + (String)status_solo);
    }

    //Solo seco
    if (valor_analogico_sensor_umidade_solo < 252 && valor_analogico_sensor_umidade_solo >= 126)
    {
      status_solo = SECO;
      Serial.println(" - Status do solo: " + (String)status_solo);
    }

    //Solo muito seco
    if (valor_analogico_sensor_umidade_solo < 126)
    {
      status_solo = MUITO_SECO;
      Serial.println(" - Status do solo: " + (String)status_solo);
    }

    lastMsg = now;

    Serial.print("is MQTT client is still connected: ");
    Serial.println(client.connected());

    // set up json object
    DynamicJsonDocument doc(1024);

    // populate keys in json
    doc["temperatura_ambiente"] = temperatura_ambiente;
    doc["umidade_ambiente"] = umidade_ambiente;
    doc["valor_sensor_umidade_solo"] = valor_analogico_sensor_umidade_solo;
    doc["status_umidade_solo"] = status_solo;

    serializeJson(doc, Serial);

    // convert json to buffer for publishing
    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    // publish
    client.publish(iothub_publish_endpoint, buffer);
  }
}
