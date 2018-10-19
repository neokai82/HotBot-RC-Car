#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "LAPTOP-QFJLVROC 8500";
const char* password = "294Bb$03";
int lastTimer =0;


const int MotorLeftReversePin = D2;
const int MotorRightReversePin = D6;
const int MotorLeftAPin = D5;
const int MotorLeftBPin = D0;
const int MotorRightAPin = D4;
const int MotorRightBPin = D3;

WiFiServer server(80);
int offFor = 500;
bool PWMOn = false;
int directionMode = 0;

void MotorLeftClockWise()
{
  digitalWrite(MotorLeftAPin, LOW);
  digitalWrite(MotorLeftBPin, HIGH);
}
void MotorLeftCounterClockWise()
{
  digitalWrite(MotorLeftAPin, HIGH);
  digitalWrite(MotorLeftBPin, LOW);
}
void MotorLeftStop()
{
  digitalWrite(MotorLeftAPin, LOW);
  digitalWrite(MotorLeftBPin, LOW);
}
void MotorRightClockWise()
{
  digitalWrite(MotorRightAPin, HIGH);
  digitalWrite(MotorRightBPin, LOW);
}
void MotorRightCounterClockWise()
{
  digitalWrite(MotorRightAPin, LOW);
  digitalWrite(MotorRightBPin, HIGH);
}
void MotorRightStop()
{
  digitalWrite(MotorRightAPin, LOW);
  digitalWrite(MotorRightBPin, LOW);
}

void MovementForward()
{
  MotorLeftClockWise();
  MotorRightClockWise();
  digitalWrite(MotorLeftReversePin, LOW);
  digitalWrite(MotorRightReversePin, LOW);
}

void MovementReverse()
{
  MotorLeftCounterClockWise();
  MotorRightCounterClockWise();
  digitalWrite(MotorLeftReversePin, HIGH);
  digitalWrite(MotorRightReversePin, HIGH);
}

void MovementTurnRight()
{
  MotorLeftCounterClockWise();
  MotorRightClockWise();
  digitalWrite(MotorLeftReversePin, LOW);
  digitalWrite(MotorRightReversePin, HIGH);
}
void MovementTurnLeft()
{
  MotorLeftClockWise();
  MotorRightCounterClockWise();
  digitalWrite(MotorLeftReversePin, HIGH);
  digitalWrite(MotorRightReversePin, LOW);
}

void MovementStop()
{
  MotorLeftStop();
  MotorRightStop();
  digitalWrite(MotorLeftReversePin, LOW);
  digitalWrite(MotorRightReversePin, LOW);
}


void PWMControl()
{
  Serial.print("Time ");
  Serial.print(millis());
  Serial.print(" directionMode ");
  Serial.print(directionMode);
  Serial.print(" PWM On ");
  Serial.print(PWMOn);
  Serial.print(" Last Timer ");
  Serial.println(lastTimer);
  if(PWMOn)
  {
    if(directionMode == 0)
    {
      MovementStop();
    }
    else if(directionMode == 1)
    {
      MovementForward();
    }
    else if(directionMode == 2)
    {
      MovementReverse();
    }
    else if(directionMode == 3)
    {
      MovementTurnLeft();
    }
    else if(directionMode == 4)
    {
      MovementTurnRight();
    }
  }
  else
  {
    MovementStop();
  }
  
  if(lastTimer < millis())
  {
    //lastTimer = millis() + offFor;
    PWMOn = 0;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(MotorLeftAPin, OUTPUT);
  pinMode(MotorLeftBPin, OUTPUT);
  pinMode(MotorRightAPin, OUTPUT);
  pinMode(MotorRightBPin, OUTPUT);
  pinMode(MotorLeftReversePin, OUTPUT);
  pinMode(MotorRightReversePin, OUTPUT);
  


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);['''[''

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

// Start the server
  server.begin();
  Serial.println("Server started");
 


  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void wifiServer()
{
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  MovementStop();
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }


  // Read the first line of the request
  String request = client.readStringUntil('\r');
  // Read the next line?
  String requestValue = client.readStringUntil('\r');
  Serial.println(request);
  Serial.println(requestValue);
  client.flush();

  // Match the request


  if (request.indexOf("/MOVE=REVERSE") != -1) {
    //MovementReverse();
    directionMode =2;
  }
  if (request.indexOf("/MOVE=LEFT") != -1) {
    //MovementTurnLeft();
    directionMode =3;
  }
  if (request.indexOf("/MOVE=RIGHT") != -1) {
    //MovementTurnRight();
    directionMode = 4;
  }
  if (request.indexOf("/SPEED=100") != -1) {
    offFor = 100;
  }
  if (request.indexOf("/SPEED=500") != -1) {
    offFor = 500;
  }
  if (request.indexOf("/SPEED=1000") != -1) {
    offFor = 1000;
  }
  if (request.indexOf("/SPEED=2000") != -1) {
    offFor = 2000;
  }

  if (request.indexOf("/MOVE=FORWARD") != -1)
  { 
    //MovementForward();
    directionMode = 1;
  }
  if (request.indexOf("/MOVE=STOP") != -1)
  {
    //MovementStop();
    directionMode = 0;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><font size=\"+12\">");
  //client.prin tln("<head><meta http-equiv=\"refresh\" content=\"1\"></head>");

  client.println("Click <a href=\"/MOVE=FORWARD\">here</a> Forward<br>");
  client.println("Click <a href=\"/MOVE=REVERSE\">here</a> Reverse<br>");
  client.println("Click <a href=\"/MOVE=STOP\">here</a> Stop<br>");

  client.println("Click <a href=\"/MOVE=LEFT\">here</a> Left<br>");
  client.println("Click <a href=\"/MOVE=RIGHT\">here</a> Right<br>");

  client.println("Click <a href=\"/SPEED=100\">here</a> 100<br>");
  client.println("Click <a href=\"/SPEED=500\">here</a> 500<br>");
  client.println("Click <a href=\"/SPEED=1000\">here</a> 1000<br>");
  client.println("Click <a href=\"/SPEED=2000\">here</a> 2000<br>");

client.println("<form action=\"/MOVE=FORWARD\">");
client.println("<input type=\"submit\" value=\"FORWARD\" />");
client.println("</form>");
client.println("<form action=\"/MOVE=STOP\">");
client.println("<input type=\"submit\" value=\"STOP\" />");
client.println("</form>");
    


  client.println("</font></html>");

  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
  MovementStop();
lastTimer = millis() + offFor;
PWMOn = 1;
}

void OTAHandler()
{
  ArduinoOTA.handle();
  delay(10);                  // delay to give the arduino time to move to the next operation.
}

void loop() {
  PWMControl();
  OTAHandler();
  wifiServer();

  
}


