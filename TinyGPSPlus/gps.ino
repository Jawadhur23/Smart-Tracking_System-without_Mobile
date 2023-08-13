#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Define the serial port for GPS module
SoftwareSerial gpsSerial(D5, D6);

// Define the TinyGPS++ object
TinyGPSPlus gps;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize serial communication for GPS module
  gpsSerial.begin(9600);
}

void loop() {
  // Read GPS data from the serial port
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Print latitude and longitude data to the serial monitor
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    }
  }
}
