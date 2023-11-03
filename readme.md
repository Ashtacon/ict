# IoT Monitoring with ESP32, LDR Sensor, and DHT

![ESP32 IoT](http://student-activity.binus.ac.id/himtek/wp-content/uploads/sites/5/2022/07/Picture1.png)

This project is an example of Internet of Things (IoT) implementation using ESP32, LDR (Light Dependent Resistor) sensor, and DHT (Digital Humidity and Temperature) sensor to measure brightness, temperature, and humidity in the surrounding environment. The data collected will be published to an MQTT (Message Queuing Telemetry Transport) broker, making it easy to monitor and access the data through various applications and devices.

## Table of Contents

- [Required Components](#required-components)
- [Setup and Configuration](#setup-and-configuration)
  - [Hardware Installation](#hardware-installation)
  - [Connection Configuration](#connection-configuration)
  - [MQTT Broker](#mqtt-broker)
- [Understanding the Code](#understanding-the-code)
  - [Library and Variable Declarations](#library-and-variable-declarations)
  - [Connection Initialization](#connection-initialization)
  - [Callback for MQTT Messages](#callback-for-mqtt-messages)
  - [Calculating Brightness](#calculating-brightness)
- [Using the Project](#using-the-project)
- [Monitoring the Data](#monitoring-the-data)
- [Troubleshooting](#troubleshooting)
- [Advanced Development](#advanced-development)

## Required Components

1. **ESP32**: ESP32 microcontroller or a similar device with WiFi and MQTT support.
2. **LDR Sensor**: Light Dependent Resistor used for measuring brightness.
3. **DHT Sensor**: DHT11 or DHT22 temperature and humidity sensor.
4. **WiFi Connection**: You need a functioning WiFi network to connect the ESP32 to and access data wirelessly.
5. **MQTT Broker**: In this example, the public broker `broker.emqx.io` is used. You can also use your own local MQTT broker or a different one.

## Setup and Configuration

### Hardware Installation

Before you start, make sure you've assembled the physical components according to the connections provided:

- The LDR sensor connected to the pin you specified in the code (in this example, pin 32).
- The DHT sensor connected to the pin you specified in the code (in this example, pin 16).

Ensure that the ESP32 is connected to the appropriate WiFi network, as specified in the code.

### Connection Configuration

You'll need to configure the WiFi connection and MQTT broker according to your needs. Here are the configuration details you should check:

```cpp
const char* ssid = "SMA ABBS Surakarta";      // WiFi network name
const char* password = "";                    // WiFi password
const char* mqtt_server = "broker.emqx.io";   // MQTT broker address
const int mqtt_port = 1883;                   // MQTT broker port
const char* mqtt_username = "emqx";           // MQTT broker username
const char* mqtt_password = "public";         // MQTT broker password
```
Make sure to replace the values above with the appropriate information for your network and MQTT broker.

## MQTT Broker

You can use the public MQTT broker mentioned in this example, broker.emqx.io, or set up your own local MQTT broker. Ensure the MQTT broker is active and accessible to your ESP32 device.

## Understanding the Code

Now let's take a closer look at the code used in this project.

## Library and Variable Declarations

In this section, we declare all the libraries used and the necessary variables:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "SMA ABBS Surakarta";            // WiFi network name
const char* password = "";                          // WiFi password
const char* mqtt_server = "broker.emqx.io";         // MQTT broker address
const int mqtt_port = 1883;                         // MQTT broker port
const char* mqtt_username = "emqx";                 // MQTT broker username
const char* mqtt_password = "public";               // MQTT broker password
const char* ldrTopic = "emqx/ldr";                  // MQTT topic for LDR
const char* brightnessTopic = "emqx/cahayas";       // MQTT topic for brightness
const char* temperatureTopic = "emqx/suhus";        // MQTT topic for temperature
const char* humidityTopic = "emqx/humds";           // MQTT topic for humidity

#define DHTPIN 16                                   // Pin used for the DHT sensor
#define DHTTYPE DHT11                               // DHT sensor type (DHT11 or DHT22)
#define LdrPin 32                                   // Pin used for the LDR sensor
```
You need to ensure that the WiFi network information and MQTT broker information match your configuration.

## Connection Initialization

In this section, the project establishes a WiFi connection and connects to the MQTT broker:

```cpp
void setup() {
  Serial.begin(9600);  // Start serial communication

  // Connect to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to the MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += WiFi.macAddress();
    Serial.printf("The Client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Public EMQX MQTT Broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  dht.begin();  // Start the DHT sensor
}
```

It's essential to ensure that the ESP32 is connected to the WiFi network before attempting to connect to the MQTT broker.

## Callback for MQTT Messages
In this project, we declare a `callback` function to handle messages received from the MQTT broker:

```cpp
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
```
This function will be called when a message is received on one of the topics you've specified.

## Calculating Brightness

In this code, there's a `calculateBrightness` function used to calculate brightness based on the LDR sensor value:

```cpp
float calculateBrightness(int ldrRawValue) {
  const float gama = 0.7;
  const float rl10 = 50;
  ldrRawValue = map(ldrRawValue, 4095, 0, 1024, 0); // Convert LDR sensor readings
  float voltase = ldrRawValue / 1024. * 5;
  float resistansi = 2000 * voltase / (1 - voltase / 5);
  float kecerahan = pow(rl10 * 1e3 * pow(10, gama) / resistansi, (1 / gama));
  return kecerahan;
}
```
This function takes the raw value from the LDR sensor and calculates brightness based on a specific formula. You can adjust this formula to match the characteristics of your LDR sensor.

## Using the Project
1. Ensure that the ESP32 is connected to your PC or computer via a USB cable.
2. Open the provided Arduino sketch.
3. Declare and set all the necessary libraries and variables according to your configuration, including the WiFi network name, password, MQTT broker address, and more.
4. Upload the code to the ESP32 by clicking the "Upload" button in the Arduino IDE.
5. Open the Serial Monitor in the Arduino IDE to monitor the results. You can view the published data and the connection status.

## Monitoring Data
To monitor the data sent by the ESP32, you can use an MQTT application that supports subscribing to MQTT topics. Here are the general steps:

1. Open an MQTT application, such as MQTT Dash, MQTT Explorer, or a similar app on your device.
2. Configure the app to connect to the MQTT broker you're using (e.g., broker.emqx.io) using the appropriate username and password.
3. Subscribe to the MQTT topics you've specified in the code, such as `emqx/ldr`, `emqx/cahayas`, `emqx/suhus`, and `emqx/humds.`
4. The app will receive data from the ESP32 and display values such as brightness, temperature, and humidity.

You can customize these topics according to your preferences.

## Troubleshooting

If you encounter issues while running this project, here are some steps you can try:

- Ensure that the ESP32 is connected to the correct WiFi network and has internet access.
- Check the MQTT broker configuration and make sure the broker is active.
- Verify the physical connections of the LDR and DHT sensors to the pins you specified in the code.
- Ensure that your device supports connecting to the MQTT broker on port 1883 (or adjust the port if necessary).
- Confirm that the MQTT broker's username and password match what you've set.

## Advanced Development

This project can be further developed by adding additional features or sensors. Some ideas for advanced development include:

- Local data storage using an SD card or EEPROM memory.
- Adding other sensors such as gas sensors, motion sensors, or a camera.
- Integrating the project with other IoT platforms like Google Cloud, AWS, or Azure.
- Using a mobile application to monitor the data remotely.

You can adapt this project to suit your needs or add additional functionality based on the ideas mentioned above.

Happy experimenting, and we hope this project helps you understand the basics of IoT development using ESP32!


