#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <HTTPClient.h>
#define USE_SERIAL Serial

char* ssid = "Kanan"; // Enter SSID
char* password = "kananwifi"; // Enter the password

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

#define DHTPIN 15 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11
#define LDR 2
#define SoilMoisture 4

const int r1 = 5;
const int r2 = 18;
float h, t;
float water; 
float LDR_VAL;
DHT dht(DHTPIN, DHTTYPE);

void setup() {  
  Serial.begin(115200); // Initialize serial
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  pinMode(r1,OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(SoilMoisture, INPUT);
  pinMode(DHTPIN,INPUT);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    delay(2500);
     h = dht.readHumidity();
     t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } else {
      Serial.print("Temperature (ºC): ");
      Serial.print(t);
      Serial.println("ºC");
      Serial.print("Humidity");
      Serial.println(h);

      if (t > 35) {
        digitalWrite(r2, HIGH);
        Serial.print("temperature (ºC): ");
        Serial.print(t);
        Serial.print("  |  ");
        Serial.print(" Fan is ON Now ");
      } else {
        digitalWrite(r2, LOW);
        Serial.print("Humidity");
        Serial.println(h);
        Serial.print("  |  ");
        Serial.print(" Fan is OFF Now ");
      }
    }

     LDR_VAL = analogRead(LDR);
    Serial.println("Light Intensity: ");
    Serial.println(LDR_VAL);

     water = digitalRead(SoilMoisture);
    if (water == HIGH) {
      digitalWrite(r1,HIGH);
      Serial.println("soil moisture Level : HIGH ");
      Serial.println("water supply: ON");
    } else {
      digitalWrite(r1,LOW); 
      Serial.println("soil moisture Level : LOW ");
      Serial.println("water supply: OFF");
    }

    lastTime = millis();
  }
if (WiFi.status() == WL_CONNECTED){
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
        //http.begin("http://192.168.9.72/sem8/post-esp-data.php"); //HTTP
        http.begin("http://192.168.2.196/sem8/post.php");
          
        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // String httpRequestData = "api_key=tPmAT5Ab3j7F9&temperature=23&humidity=89&moisture=89";
        String httpRequestData = "temperature=" + String(t) + "&humidity=" + String(h) + "&LDR=" + String(LDR_VAL) + "&Soilmoisture=" + String(water);

        int httpCode = http.POST(httpRequestData);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
}
delay(7000);

}
