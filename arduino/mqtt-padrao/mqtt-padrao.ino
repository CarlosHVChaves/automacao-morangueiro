#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>

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
    //
  while (true)
  {
Serial.println("publishing string ");
  boolean rc = mqttClient.publish("iot-2/evt/telemetria/fmt/json","{\"temp\":50}");
  Serial.println("publishing Temp");
   //rc = mqttClient.publish("testbyte", outmsg,2);
    delay(1000); //wait
    mqttClient.loop(); //call loop
  }
    
  }
