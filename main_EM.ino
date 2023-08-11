#include "EM_lite.hpp"
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

//variables y objetos varios
EMlite em(2, 22);
float* data;
unsigned long ultima, ultima1;

//CREDENCIALES WIFI Y MQTT
const char* ssid = "Notebooks ETEC";
const char* password = "373k123*";
IPAddress server(192,168,40,242);
#define MQTT_port 1883
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

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
    if (mensaje == "SLEEP"){
        Serial.print("Sleep moment");
        ESP.deepSleep(900000000);
    }
}

void reconnect() {
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "MadBone";
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
            Serial.println("Conectado!!");
            // Once connected, publish an announcement...
            client.publish("test", "papitas");
            // ... and resubscribe
            client.subscribe("test");
            client.subscribe("hum");
            client.subscribe("temp");
            client.subscribe("sleep"); 
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
        }
    }
}

void setup(){
    Serial.begin(115200);
    //WIFI & MQTT SETUP
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

    //OTA SETUP
    ArduinoOTA.setHostname("iDareYouToChangeThisCodeYouFilthyCasual");
    ArduinoOTA.setPassword("SunnySunday");
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void loop(){
    //OTA & MQTT HANDLER
    ArduinoOTA.handle();
    if (!client.connected()){
        if (millis()>=ultima1){
            ultima1 = millis() + 2000;
            reconnect();
        }
    }
    client.loop();

    if (millis()>=ultima){
        ultima = millis() + 10000;
        data = em.get_data();
        Serial.println("----------------------------------------\n\t\tDATA\n----------------------------------------");
        Serial.print("BMP TEMP:\t"); Serial.println(*(data + 0));
        Serial.print("DHT TEMP:\t"); Serial.println(*(data + 1));
        Serial.print("DHT HUM:\t"); Serial.println(*(data + 2));
        Serial.print("BMP BAR:\t"); Serial.println(*(data + 3));
        
        if (client.connected()){   
            snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", *(data + 0));
            Serial.print("Mensaje Mandado T: ");
            Serial.println(msg);
            client.publish("temp", msg);

            snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", *(data + 2));
            Serial.print("Mensaje Mandado H: ");
            Serial.println(msg);
            client.publish("hum", msg);

            snprintf (msg,  MSG_BUFFER_SIZE, "%.2f", *(data + 3));
            Serial.print("Mensaje Mandado bp: ");
            Serial.println(msg);
            client.publish("bp", msg);
        }
    }
}