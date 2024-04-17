#include <SoftwareSerial.h>

#include <DHT22.h>
#include <dht11.h>

#define DHT11PIN 5

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6n-P2q_Qz"
#define BLYNK_TEMPLATE_NAME "Monitoring Silo Beras dan Gudang Beras"
#define BLYNK_AUTH_TOKEN "YHh7hJUM9vT10allGWULDNRhZoaQ03d6"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ya gak punya kuota ya ? wkwkwkwk";
char pass[] = "hahahahaha";
char message[50] = "";

int data = 4;
float nilaiCO;

DHT22 dht22(data);

dht11 DHT11;

BlynkTimer timer;
SoftwareSerial sendMQ(35, 34);

void sendALL()
{
  MQ_Sensor();
  // Read temperature and humidity from DHT22 sensor
  float siloTemp = dht22.getTemperature();
  float siloHum = dht22.getHumidity();

  // Print temperature and humidity from DHT22 sensor
  Serial.print("Humidity Silo: ");
  Serial.println(siloHum);
  Serial.print("Temp Silo: ");
  Serial.println(siloTemp);

  // Read temperature and humidity from DHT11 sensor
  int chk = DHT11.read(DHT11PIN);

  // Print temperature and humidity from DHT11 sensor
  Serial.print("Humidity Gudang (%): ");
  float gudangHum = (float)DHT11.humidity;
  Serial.println(gudangHum, 2);
  Serial.print("Temperature Gudang (C): ");
  float gudangTemp = (float)DHT11.temperature;
  Serial.println(gudangTemp, 2);

  Serial.print("Nilai CO (ppm):");
  Serial.println(nilaiCO);

  Serial.println();
  Serial.println();

  Blynk.virtualWrite(V0, siloTemp);
  Blynk.virtualWrite(V1, siloHum);
  Blynk.virtualWrite(V2, gudangTemp);
  Blynk.virtualWrite(V3, gudangHum);
  Blynk.virtualWrite(V4, message);
}

void MQ_Sensor()
{
  String receivedMQ = sendMQ.readString();

  if (receivedMQ.length() > 0)
  {
    nilaiCO = receivedMQ.toFloat();
  }

  if (nilaiCO <= 5.0)
  {
    snprintf(message, sizeof(message), "Gudang Aman, CO: %.2f ppm", nilaiCO);
  }
  else if (nilaiCO <= 10.0)
  {
    snprintf(message, sizeof(message), "Ada Asap, Indikasi Kebakaran, CO: %.2f ppm", nilaiCO);
  }
  else if (nilaiCO <= 25.0)
  {
    snprintf(message, sizeof(message), "!!TERJADI KEBAKARAN!! Hubungi 113, CO: %.2f ppm", nilaiCO);
  }
  else
  {
    snprintf(message, sizeof(message), "!!! KEBAKARAN !! - !!AMANKAN GUDANG!!, CO: %.2f ppm", nilaiCO);
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  sendMQ.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, sendALL);
}

void loop()
{
  Blynk.run();
  timer.run();
}
