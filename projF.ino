#include <WiFi.h>
#include <PubSubClient.h>

// Pines de relés
#define RELAY1 27
#define RELAY2 12
#define RELAY3 14

// WiFi
const char* ssid = "galileo";
const char* password = "";

// MQTT
const char* mqtt_server = "galiot.galileo.edu";
const char* mqtt_user = "monair";
const char* mqtt_pass = "MONair2023";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(1000);
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == "iot/carga1") {
    digitalWrite(RELAY1, message == "ON" ? LOW : HIGH);
    Serial.println("Carga 1 actualizada");
  } else if (String(topic) == "iot/carga2") {
    digitalWrite(RELAY2, message == "ON" ? LOW : HIGH);
    Serial.println("Carga 2 actualizada");
  } else if (String(topic) == "iot/carga3") {
    digitalWrite(RELAY3, message == "ON" ? LOW : HIGH);
    Serial.println("Carga 3 actualizada");
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Conectando a MQTT...");
    if (client.connect("ESP32Johan", mqtt_user, mqtt_pass)) {
      Serial.println("¡Conectado a MQTT!");

      client.subscribe("iot/carga1");
      client.subscribe("iot/carga2");
      client.subscribe("iot/carga3");
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(client.state());
      Serial.println("Intentando de nuevo en 1s...");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);

  digitalWrite(RELAY1, HIGH);  // Apagado por defecto
  digitalWrite(RELAY2, HIGH);
  digitalWrite(RELAY3, HIGH);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}
