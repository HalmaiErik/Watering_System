#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Water Sensor
#define ALERT_WATER_LEVEL 300
const int waterSensorPin = A0;
const int waterAlertLed = D1;
int waterSensorValue = 0;

// Uno connection
SoftwareSerial s(D6, D5);  // RX, TX

// WiFi
AsyncWebServer server(80);
const char* ssid = "Orange-69vy";
const char* password = "wfEZ9myy";
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input: <input type="text" name="input">
    <input type="submit" value="Submit">
  </form>
  </body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  // Water Sensor
  pinMode(waterSensorPin, INPUT);
  pinMode(waterAlertLed, OUTPUT);

  // Uno connection
  s.begin(9600);

  // WiFi
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputTime;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("input")) {
      inputTime = request->getParam("input")->value();
    }
    else {
      inputTime = "No message sent";
    }
    //Serial.println(inputTime);
    if (!inputTime.equals("No message sent")) {
      char t[10];
      inputTime.toCharArray(t, 10);
      t[8] = '_';
      t[9] = '\0';
      Serial.println(t);
      s.write(t);
    }
    request->send(200, "text/html", "Irrigation will happen every " + inputTime +
                                "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}
void loop() {
  waterSensorValue = analogRead(waterSensorPin);
  delay(50);

  if (waterSensorValue < ALERT_WATER_LEVEL) {
      digitalWrite(waterAlertLed, HIGH);
  }
  else {
    digitalWrite(waterAlertLed, LOW);
  }
}
