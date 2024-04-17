#include <SoftwareSerial.h>
#include <MQUnifiedsensor.h>

// Definitions
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0                 // Analog input 0 of your arduino
#define type "MQ-135"          // MQ135
#define ADC_Bit_Resolution 10  // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6 // RS / R0 = 3.6 ppm
// #define calibration_button 13 //Pin to calibrate your sensor

#define buzzerPIN 4

// Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
SoftwareSerial sendMQ(2, 3);

float CO_Temp = 0;
float CO_Sensor = 0;
float CO_Baru;

void setup()
{
  // Init the serial port communication - to debug the library
  Serial.begin(9600); // Init serial port
  sendMQ.begin(9600);

  // Set math model to calculate the PPM concentration and the value of constants
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setA(605.18);
  MQ135.setB(-3.937); // Configure the equation to to calculate NH4 concentration

  MQ135.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0))
  {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0)
  {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }

  pinMode(buzzerPIN, OUTPUT);
}

void readSensor()
{
  MQ135.update();
  CO_Sensor = MQ135.readSensor();
  CO_Temp += CO_Sensor;
}

void filterSensor()
{
  for (int i = 0; i < 20; i++)
  {
    readSensor();
    delay(10);
  }
  CO_Baru = CO_Temp / 20;
  CO_Temp = 0;
}

void buzzer()
{
  // Menyalakan buzzer
  digitalWrite(buzzerPIN, HIGH);
  delay(1250); // Menyalakan buzzer selama 1 detik

  // Mematikan buzzer
  digitalWrite(buzzerPIN, LOW);
  delay(100); // Menunggu 1 detik sebelum mengulangi proses
}

void loop()
{
  filterSensor();
  Serial.println(CO_Baru);
  sendMQ.println(CO_Baru);

  if (CO_Baru >= 5.0)
  {
    buzzer();
  }
  delay(750); // Sampling frequency
}