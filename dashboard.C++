#include <LiquidCrystal.h>
#include<DHT.h>

#define DHTPIN A0    
#define DHTTYPE DHT11   

// Initialize the LiquidCrystal library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DHT dht(DHTPIN, DHTTYPE);


// Define the pin for the IR sensor
int irSensorPin = 8;  // IR sensor connected to digital pin 8

// Variables to store time for one full wheel rotation
unsigned long startTime = 0;
unsigned long endTime = 0;
float timeDifference = 0;

// Car wheel specifications (adjust these values for your car)
float wheelRadius = 0.13;                              // Wheel radius in meters (example: 30 cm)
float wheelCircumference = 2 * 3.14159 * wheelRadius;  // Calculate the circumference

// Threshold for determining if speed is effectively zero
const float speedThreshold = 0.1;  // Adjust as needed

void setup() {
  // Initialize the Serial Monitor
  Serial.begin(9600);
  dht.begin();

  // Set the IR sensor pin as input
  pinMode(irSensorPin, INPUT);

  // Initialize the LCD and specify the dimensions (16 columns, 2 rows)
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Car Speed Monitor");
}

void loop() {

    delay(2000);
    float temp = dht.readTemperature();
    if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  }else{
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("°C");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("°C");
  }

  // Wait for the wheel rotation (object blocking the sensor)
  if (digitalRead(irSensorPin) == LOW) {
    // If it's the first detection, record the start time
    if (startTime == 0) {
      startTime = millis();
      Serial.println("Wheel detected: Start time recorded");
    } else {
      // Record the end time for the next rotation
      endTime = millis();

      // Calculate the time difference between two rotations (in seconds)
      timeDifference = (endTime - startTime) / 1000.0;
      startTime = endTime;  // Reset startTime for the next rotation

      // Calculate RPM (Revolutions per minute)
      float rpm = 60.0 / timeDifference;

      // Calculate the wheel rotation speed in meters per second
      float rotationsPerSecond = 1.0 / timeDifference;        // Number of rotations per second
      float speed = rotationsPerSecond * wheelCircumference;  // Speed in meters per second

      // Convert speed to km/h
      float speedKmH = speed * 3.6;  // 1 m/s = 3.6 km/h

      // Print the RPM and Speed to the Serial Monitor
      Serial.print("Speed: ");
      Serial.print(speedKmH);
      Serial.println(" km/h");
      Serial.print("RPM: ");
      Serial.println(rpm);

      // Display the speed and RPM on the LCD screen
      lcd.clear();
      lcd.setCursor(0, 0);
      //lcd.print("S:");
      lcd.print(speedKmH * 10);
      lcd.print("km/h ");

      // Move to second line to display RPM
      // lcd.setCursor(0, 1);
      //lcd.print("R:");
      lcd.print(rpm * 10);

      // Add a small delay to avoid multiple readings for the same rotation
      delay(500);

      // Check if speed is effectively zero
      static float previousSpeed = speedKmH;
      float speedDifference = abs(speedKmH - previousSpeed);
      float a = speedKmH;
      delay(2000);
      float b = speedKmH;
      if (a == b)
      // if (speedDifference <= speedThreshold && digitalRead(irSensorPin) == LOW)

      {
        // Speed is effectively zero
        speedKmH = 0;
        rpm = 0;

        // Update the LCD display
        lcd.clear();
        lcd.setCursor(0, 0);
        //lcd.print("S:");
        lcd.print(speedKmH * 10);
        lcd.print(" km/h ");
        // lcd.setCursor(0, 1);
        //lcd.print("R:");
        lcd.print(rpm * 10);
      }

      previousSpeed = speedKmH;
    }
  }
}