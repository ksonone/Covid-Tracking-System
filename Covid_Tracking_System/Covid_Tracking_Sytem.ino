#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/*
   Covid Tracking System 
   Author : Kapil H. Sonone
*/

const char* ssid = "Your WiFi Name";          // Modify this field with right credential
const char* password = "Your WiFi Password";  // Modify this field with right credential

const char* host = "kearnux.000webhostapp.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "5B FB D1 D4 49 D3 0F A9 C6 40 03 34 BA E0 24 05 AA D2 E2 01";

char *myStrings[] = {"Mumbai","Pune", "Nagpur", "Amravati"}; // Name of cities

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  for (int i = 0; i < 4; i++)
  {
    delay(1000);

    WiFiClient client;
    const int httpPort = 80;

    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    String url = "/index.php";

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(2000);

    while (client.available()) {
      String line = client.readString();

      const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
      DynamicJsonBuffer jsonBuffer(capacity);

      // Parse JSON object
      JsonObject& root = jsonBuffer.parseObject(line.substring(276));
      if (!root.success()) {
        Serial.println(F("Parsing failed!"));
        return;
      }

      // Decode JSON/Extract values
      Serial.println(F("Response:"));
      Serial.println(myStrings[i]);
      Serial.print("Confirmed : "); 
      Serial.println(root[myStrings[i]]["confirmed"].as<char*>());
      Serial.print("Death : ");
      Serial.println(root[myStrings[i]]["death"].as<char*>());
      Serial.print("Recovered : ");
      Serial.println(root[myStrings[i]]["recovered"].as<char*>());
      jsonBuffer.clear();

    }
    Serial.println();
  }
}
