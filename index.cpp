#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "SMA ABBS Surakarta";
const char* password = "";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_username = "emqx";
const char* mqtt_password = "public";
const char* ldrTopic = "emqx/ldr";
const char* brightnessTopic = "emqx/cahayas";
const char* temperatureTopic = "emqx/suhus";
const char* humidityTopic = "emqx/humds";

#define DHTPIN 16
#define DHTTYPE DHT11
#define LdrPin 32

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += WiFi.macAddress();
    Serial.printf("The Client %s connects to public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT Broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  dht.begin();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

float calculateBrightness(int ldrRawValue) {
  const float gama = 0.7;
  const float rl10 = 50;
  ldrRawValue = map(ldrRawValue, 4095, 0, 1024, 0); //mengubah nilai pembacaan sensor LDR dari nilai ADC arduino menjadi nilai ADC ESP32
  float voltase = ldrRawValue / 1024.*5;
  float resistansi = 2000 * voltase / (1-voltase/5);
  float kecerahan = pow(rl10*1e3*pow(10,gama)/resistansi,(1/gama));
  return kecerahan;
}

void loop() {
  client.loop();

  int ldrRawValue = analogRead(LdrPin);
  float brightness = calculateBrightness(ldrRawValue);

  Serial.print("LDR Value: ");
  Serial.println(ldrRawValue);
  Serial.print("Brightness: ");
  Serial.println(brightness);

  // Publish LDR value and brightness to their respective topics
  client.publish(ldrTopic, String(ldrRawValue).c_str());
  client.publish(brightnessTopic, String(brightness).c_str());

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature)) {
    // Publish temperature and humidity to their respective topics
    client.publish(temperatureTopic, String(temperature).c_str());
    client.publish(humidityTopic, String(humidity).c_str());
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }

  delay(5000); // Adjust the delay as needed
}