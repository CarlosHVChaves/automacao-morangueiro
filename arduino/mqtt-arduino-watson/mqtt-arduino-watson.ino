#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <ArduinoJson.h>
// #include <MQTTClient.h>
// #include <IPStack.h>
// #include <Countdown.h>

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

// variáveis que quardaram os valores dos sensores
int umidade_ambiente = 0;
int temperatura_ambiente = 0;
int valor_analogico_sensor_umidade_solo = 0;
int umidade_solo = 0;
char* status_solo = "";

// variável para montar o JSON
// char* data;
// StaticJsonBuffer<256> jsonBuffer;
// String deviceEvent;
String monta_json;

EthernetClient ethClient;
//IPStack ipstack(ethClient);

//MQTT::Client<IPStack, Countdown, 100, 1> client = MQTT::Client<IPStack, Countdown, 100, 1>(ipstack);

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
// IPAddress ip(192, 168, 1, 61);
IPAddress server(169, 62, 202, 130); // "mc6re7.messaging.internetofthings.ibmcloud.com"

PubSubClient mqttClient(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
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
  mqttClient.setCallback(callback);
  if (mqttClient.connect("d:mc6re7:arduino:a2560", "use-token-auth", "12345678")) {
    // connection succeeded
    Serial.println("Connected ");
    // boolean r= mqttClient.subscribe("test");
    // Serial.println("subscribe ");
    // Serial.println(r);
    if (mqttClient.subscribe("iot-2/cmd/ligar_bomba/fmt/json")) {
      Serial.println("Inscrito no tópico"); 
    } 
    else {
      Serial.println("Falha ao se conectar no tópico");
    }
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
    umidade_ambiente = dht.readHumidity();
    temperatura_ambiente = dht.readTemperature();
    // testa se retorno é valido, caso contrário algo está errado.
    if (isnan(temperatura_ambiente) || isnan(umidade_ambiente)) 
    {
      Serial.println("Failed to read from DHT");
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

    //Solo umido
    if (valor_analogico_sensor_umidade_solo >= 452)
    {
      status_solo = "úmido";
      Serial.println(" - Status do solo: "+(String)status_solo);
    }
 
    //Solo com umidade moderada
    if (valor_analogico_sensor_umidade_solo < 452 && valor_analogico_sensor_umidade_solo >= 226)
    {
      status_solo = "umidade moderada";
      Serial.println(" - Status do solo: "+(String)status_solo);
    }
 
    //Solo seco
    if (valor_analogico_sensor_umidade_solo < 226)
    {
      status_solo = "seco";
      Serial.println(" - Status do solo: "+(String)status_solo);  
    }
    // Comunicação MQTT
    Serial.println("publishing string ");
    //json = "{\"temp_ar\":{" + temperatura_ambiente + "},\"umi_ar\":{" + umidade_ambiente + "},\"status_solo\":{" + status_solo + "}}";

    // char* gravar_temperatura_ambiente = "\"temperatura_ambiente\": " + temperatura_ambiente;
    // char* gravar_umidade_ambiente = ", \"umidade_ambiente\": " + umidade_ambiente;
    // String gravar_status_solo = ", \"status_umidade_solo\": " +(String)status_solo;
    // String monta_json = (String)gravar_temperatura_ambiente + (String)gravar_umidade_ambiente + gravar_status_solo;
    // Serial.println(monta_json);

    // String payload = "{ \"devices\": \"*\",\"payload\": {" + montar_json + "}}";
    // String payload = "{" + monta_json + "}";
    // payload.toCharArray(data, (payload.length() + 1));

    // MQTT::Message message;
    // message.qos = MQTT::QOS0;
    // message.retained = false;
    // deviceEvent = String("{\"d\":{\"myName\":\"Arduino Mega\",\"temperature\":");
    // char buffer[60];
    // dtostrf(temperatura_ambiente,1,2, buffer);
    // deviceEvent += buffer;
    // deviceEvent += "}}";
    // Serial.print("\t");
    // Serial.print(buffer);
    // Serial.print("\t\t");
    // deviceEvent.toCharArray(buffer, 60);
    // Serial.println(buffer);
    // message.payload = buffer;
    // message.payloadlen = strlen(buffer);

    DynamicJsonDocument doc ( 1024 );
    doc [ "temperatura_ambiente" ] = temperatura_ambiente ;
    doc [ "umidade_ambiente" ] = umidade_ambiente ;
    doc [ "valor_sensor_umidade_solo" ] = valor_analogico_sensor_umidade_solo;
    doc [ "status_umidade_solo" ] = status_solo ;  

    serializeJson (doc, Serial);
    // serializeJson(doc, monta_json);
    // char* string2char(String command){
    //   if(command.length()!=0){
    //     char *p = const_cast<char*>(command.c_str());
    //     return p 
    //   }
    // }
    // char* json_char = monta_json;
    // Isso imprime: 
    // {"sensor": "gps", "tempo": 1351824120, "dados": [48.756080,2.302038]}

    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    mqttClient.publish("iot-2/evt/telemetria/fmt/json", buffer, n);
    // Serial.println(json);
    // Serial.println("{\temperatura_ar
    // boolean rc = mqttClient.publish("iot-2/evt/telemetria/fmt/json","{\"temperatura ambiente\":\"20\"}");
    // boolean rc = mqttClient.publish("iot-2/evt/telemetria/fmt/json",json)
    // boolean rc = mqttClient.publish("iot-2/evt/telemetria/fmt/json",F(monta_json));
    
    delay(1000); //wait
    mqttClient.loop(); //call loop
  }
}
