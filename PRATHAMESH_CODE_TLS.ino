
#include <SPI.h>
#include <EthernetLarge.h>
#include <SSLClient.h>
#include "certificates.h" // This file must be regenerated
#include <PubSubClient.h>

const char my_cert[] = "FIXME";
const char my_key[] = "FIXME";
SSLClientParameters mTLS = SSLClientParameters::fromPEM(my_cert, sizeof my_cert, my_key, sizeof my_key);

byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
const char* mqttServer = "broker.example"; // Broker address
IPAddress ip (192, 168, 1, 2); // Custom client static IP

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
SSLClient ethClientSSL(ethClient, TAs, (size_t)TAs_NUM, A5);
PubSubClient client(mqttServer, 8883, callback, ethClientSSL);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(){
  // Start Serial
  Serial.begin(115200);
  while(!Serial);
  // Enable mutual TLS with SSLClient
  ethClientSSL.setMutualAuthParams(mTLS);
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  Ethernet.begin(mac, ip);
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}