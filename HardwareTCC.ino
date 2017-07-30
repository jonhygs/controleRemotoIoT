
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>


const char* ssid     = "Casa_Inteligente";
const char* password = "0kBkrtgvc&&&&";
const char* broker = "192.168.0.149";

WiFiClient IRComander;
PubSubClient MQTT(IRComander);

char msg[50] = "1";
IRsend irsend(5);
int value = 0;

// Função para conectar no WiFi
void setupWiFi(){
  Serial.println();
  Serial.println();
  Serial.print("Conectando em  ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");  
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(topic[3] == 'l'){
    Serial.println("Ligando a TV");
    irsend.sendNEC(0x20DF10EF, 32);
  }
  if(topic[3] == 'c'){
    Serial.println("Mudando Canal");
    irsend.sendNEC(0x20DF00FF, 32);
  }
  if(topic[6] == 'A'){
    Serial.println("Aumentar Volume");
    irsend.sendNEC(0x20DF40BF, 32);
  }
  if(topic[6] == 'D'){
    Serial.println("Diminuir Volume");
    irsend.sendNEC(0x20DFC03F, 32);
  }
}

void reconectar(){
   while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT.");
    if (MQTT.connect("ESP8266999")) {
      Serial.println("Conectado com Sucesso ao Broker");
      MQTT.subscribe("TV/ligar");
      Serial.println("Inscrito no tópico com sucesso!");
      MQTT.subscribe("TV/volA");
      Serial.println("Inscrito no tópico com sucesso!");
      MQTT.subscribe("TV/volD");
      MQTT.subscribe("TV/canal"); 
    } else {
      Serial.print("Falha ao Conectador, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tentando se reconectar...");
      delay(3000);
    }
  }
}
void setup() {
  Serial.begin(115200);
  delay(10);
  setupWiFi();
  MQTT.setServer(broker,1883);
  MQTT.setCallback(callback);
  irsend.begin();
  pinMode(14,OUTPUT);

}

void loop() {
  if (!MQTT.connected()){
    reconectar();
  }
  MQTT.loop();
   
}

