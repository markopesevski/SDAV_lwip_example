int sensorPin = A0;
int ledPin = 13;
int sensorValue = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  Serial.println(analogRead(sensorPin));
  delay(1000);
}
