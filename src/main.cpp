#include <Arduino.h>

void setupTimer4();
void setFanSpeed(uint8_t percent);
bool loadConfigFromSD();

void setupTimer4()
{
  pinMode(6, OUTPUT);
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0;
  TCCR4A |= (1 << COM4A1);
  TCCR4A |= (1 << WGM41);
  TCCR4B |= (1 << WGM43) | (1 << WGM42);
  TCCR4B |= (1 << CS41); // Prescaler = 8
  ICR4 = 640;            // TOP value for ~25kHz PWM
  OCR4A = 0;             // Initial duty cycle = 0%
}

void setFanSpeed(uint8_t percent)
{
  percent = constrain(percent, 0, 100);
  OCR4A = map(percent, 0, 100, 0, ICR4);
}

void setup()
{
  Serial.begin(9600);
  delay(1000); // Wait for serial monitor
  setupTimer4();

  if (!loadConfigFromSD())
  {
    Serial.println("Using fallback defaults.");
  }

  extern struct FanConfig config;
  setFanSpeed(config.defaultDuty);
}

void loop()
{
  // For now, idle
}
