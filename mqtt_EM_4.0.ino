#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BMP085.h>


int LED = 14;

#define DHTPIN 2
#define DHTTYPE    DHT22
DHT dht(DHTPIN, DHTTYPE);
float temp;
int hum;

Adafruit_BMP085 bmp;
float presion;

unsigned long now;
unsigned long ultima;

const char* ssid = "Notebooks ETEC";
//const char* password = "ponecualquiera";
const char* password = "373k123*";
//IPAddress server(192, 168, 54, 224);  //MQTT server IP
IPAddress server(192,168,40,241);
#define MQTT_port 1883

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String mensaje;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    mensaje += (char)payload[i];
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (mensaje == "1") {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.printf("led encendido");
  } else if(mensaje == "2"){
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage HIGH
    Serial.print("led apagado");
  }
  if (mensaje != "" && mensaje != "1" && mensaje != "2"){
    Serial.print("Sleep moment");
    ESP.deepSleep(900000000);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Joseph";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!!");
      // Once connected, publish an announcement...
      client.publish("test", "papitas");
      client.publish("temp", "");
      client.publish("hum", "");
      client.publish("led", "0");
      // ... and resubscribe
      client.subscribe("test");
      client.subscribe("hum");
      client.subscribe("temp");
      client.subscribe("led"); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  dht.begin();

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado con éxito, mi IP es: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  
  client.setServer(server, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (millis()>=ultima){
    ultima = millis() + 2000;
    presion = bmp.readPressure() * 0.01;
    float t = bmp.readTemperature();
    float h = dht.readHumidity();
    while(/*isnan(t) ||*/ isnan(h)){
      //t = dht.readTemperature();
      h = dht.readHumidity();
    }
    snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", t);
    Serial.print("Mensaje Mandado T: ");
    Serial.println(msg);
    client.publish("temp", msg);

    snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", h);
    Serial.print("Mensaje Mandado H: ");
    Serial.println(msg);
    client.publish("hum", msg);

    snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", presion);
    Serial.print("Mensaje Mandado bp: ");
    Serial.println(msg);
    client.publish("bp", msg);
  }
}