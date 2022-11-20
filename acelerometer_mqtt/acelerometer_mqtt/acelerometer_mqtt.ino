/*
  Sistemas Distribuidos e de Tempo Real - UNISINOS 2022/2
  Davi Schmitz


  /** 
  * Local broker that accept connections and one internal client
  *
  * 
  *  +-----------------------------+
  *  | ESP                         |
  *  |                 +--------+  | 1883 <--- External client/s
  *  |       +-------->| broker |  |
  *  |       |         +--------+  |
  *  |       |                     |
  *  |       |                     |
  *  |       |                     |     -----
  *  |       v                     |      ---  
  *  | +----------+                |       -  
  *  | | internal |                |+------*  Wifi
  *  | | client   |                |          
  *  | +----------+                |          
  *  |                             |
  *  +-----------------------------+
  * 
*/

#include <TinyMqtt.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

const char* ssid = "";
const char* password = "";

std::string ax_topic = "esp32/ax";
std::string ay_topic = "esp32/ay";
std::string az_topic = "esp32/az";

MqttBroker broker(1883);
MqttClient client(&broker);

void onPublish(const MqttClient* /* source */, const Topic& topic, const char* payload, size_t /* length */) {
  Serial << "--> client received from: " << topic.c_str() << ": " << payload << endl;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial << "Connecting to Wifi " << endl;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial << '-';
    delay(500);
  }

  Serial << "Connected to " << ssid << " IP address: " << WiFi.localIP() << endl;

  broker.begin();

  client.setCallback(onPublish);
  client.subscribe(ax_topic);
  client.subscribe(ay_topic);
  client.subscribe(az_topic);

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
}

void loop() {
  broker.loop();
  client.loop();

  static const int publishInterval = 500;  // ms
  static uint32_t timer = millis() + publishInterval;

  if (millis() > timer) {
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.println("");
    Serial << "Client is publishing to: " << ax_topic.c_str() << endl;
    client.publish(ax_topic,  std::string(String(a.acceleration.x).c_str()));

    Serial.println("");
    Serial << "Client is publishing to: " << ay_topic.c_str() << endl;
    client.publish(ay_topic,  std::string(String(a.acceleration.y).c_str()));

    Serial.println("");
    Serial << "Client is publishing to: " << az_topic.c_str() << endl;
    client.publish(az_topic,  std::string(String(a.acceleration.z).c_str()));
    timer += publishInterval;
  }
}