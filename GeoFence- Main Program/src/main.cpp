#include <Arduino.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"

#define M_PI 3.14159265358979323846264338327950288
#define rxGPS 4
#define txGPS 5

const double fences[1][10][2] = {{{19.064392, 72.835376},
                                  {19.064615, 72.835225},
                                  {19.064731, 72.835606},
                                  {19.064775, 72.836104},
                                  {19.064537, 72.836110},
                                  {19.064374, 72.836100},
                                  {19.064265, 72.836105},
                                  {19.064280, 72.835744},
                                  {19.064293, 72.835522},
                                  {19.064378, 72.835403},}
};

/*Variables to store AP credentials*/
String ssid = "Oneplus";
String password = "12345678";
int WiFiConnectMode = 1; /* 0: smart config; 1: hard code*/

double latitude, longitude;
int sat;
String date;
char lati[12];
char longi[12];
int targetStatus;
int fence;
char cumulativeAngle[12];
int deviceStatus = 0;

SoftwareSerial gpsSerial(rxGPS, txGPS);
TinyGPSPlus gps;
ESP8266WebServer gpsServer(80);

void connectWifi();
void updateLatLon();
void pip();
void handleRoot();
void fenceSelect();
void gps_data();

void setup(){
    Serial.begin(9600);
    gpsSerial.begin(9600);
    connectWifi();

    gpsServer.on("/", handleRoot);
    gpsServer.on("/status", fenceSelect);
    gpsServer.on("/values", gps_data);
    gpsServer.begin();
}
 
void loop(){
    while (gpsSerial.available()){
        deviceStatus = 1;
        updateLatLon();
        pip();        
        gpsServer.handleClient();
    }
    gpsServer.handleClient();
}

void connectWifi(){
    if(WiFiConnectMode == 0){
        // Operate the ESP12E in wifi station mode for smart config
        WiFi.mode(WIFI_STA);

        // Begin the smart configuration to get the Access Point credentials
        WiFi.beginSmartConfig();
        Serial.println("------------------------------------------------");
        Serial.print("Waiting for SmartConfig ");
        while (!WiFi.smartConfigDone()) {
            delay(250);
            Serial.print(".");
        }
        Serial.println();
        Serial.println("SmartConfig done.");

        // Print the AP credentials to the serial monitor
        ssid = WiFi.SSID();
        password = WiFi.psk();
        Serial.println("------------------------------------------------");
        Serial.print("Acesspoint SSID : ");
        Serial.println(ssid);
        Serial.print("Acesspoint password : ");
        Serial.println(password);
        Serial.println("------------------------------------------------");

        // Connect the ESP12E to the AP
        Serial.print("Connecting to Access Point ");
        while (WiFi.status() != WL_CONNECTED) {
            delay(100);
            Serial.print(".");
        }
        Serial.println();
        Serial.println("Connected.");
        Serial.println("------------------------------------------------");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.println("------------------------------------------------");
    }
    else{
        String ssid = "Oneplus"; //Access point ssid
        String password = "12345678"; //Access point password
        WiFi.begin(ssid,password);
        Serial.println("------------------------------------------------");
        Serial.print("Connecting to Access Point ");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("------------------------------------------------");
    }
}

void updateLatLon(){
    if (gps.encode(gpsSerial.read()))
    {  
      sat = gps.satellites.value();
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      dtostrf(latitude,9,7,lati);
      dtostrf(longitude,9,7,longi);
      Serial.print("SATS: ");
      Serial.println(sat);
      Serial.print("LAT: ");
      Serial.println(latitude,6);
      Serial.print("LONG: ");
      Serial.println(longitude,6);
      Serial.print("ALT: ");
      Serial.println(gps.altitude.meters());
      Serial.print("SPEED: ");
      Serial.println(gps.speed.mps());
 
      Serial.print("Date: ");
      date = String(gps.date.day())+"/"+gps.date.month()+"/"+gps.date.year();
      Serial.println(date);
 
      Serial.print("Hour: ");
      Serial.print(gps.time.hour()); Serial.print(":");
      Serial.print(gps.time.minute()); Serial.print(":");
      Serial.println(gps.time.second());
      Serial.println("---------------------------");
      delay(1000);
    }
}

void pip(){
    int fenceSize = sizeof(fences[fence - 1])/sizeof(fences[fence - 1][0]);
    double vectors[fenceSize][2];
    for(int i = 0; i < fenceSize; i++){
        vectors[i][0] = fences[fence - 1][i][0] - latitude;
        vectors[i][1] = fences[fence - 1][i][1] - longitude;
    }
    double angle = 0;
    double num, den;
    for(int i = 0; i < fenceSize; i++){
        num = (vectors[i%fenceSize][0])*(vectors[(i+1)%fenceSize][0])+ (vectors[i%fenceSize][1])*(vectors[(i+1)%fenceSize][1]);
        den = (sqrt(pow(vectors[i%fenceSize][0],2) + pow(vectors[i%fenceSize][1],2)))*(sqrt(pow(vectors[(i+1)%fenceSize][0],2) + pow(vectors[(i+1)%fenceSize][1],2)));
        angle = angle + (180*acos(num/den)/M_PI);
    }
    dtostrf(angle,9,7,cumulativeAngle);
    if(angle > 355 && angle < 365)
        targetStatus = 1;
    else
        targetStatus = 0;
}

void handleRoot(){
    String s = webpage;
    gpsServer.send(200, "text/html", s);
}

void fenceSelect(){
    fence = gpsServer.arg("fenceValue").toInt();
    gpsServer.send(200, "text/plane", String(fence));
}

void gps_data(){
    String payload = String(sat) + "#" + date + "#" + lati + "#" + longi;
    if(targetStatus == 0)
        payload = payload + "#outside";
    else
        payload = payload + "#inside";
    payload = payload + "#" + cumulativeAngle;
    if(deviceStatus == 0)
        payload = payload + "#offline";
    else
        payload = payload + "#online";
    gpsServer.send(200, "text/plane", payload);
}