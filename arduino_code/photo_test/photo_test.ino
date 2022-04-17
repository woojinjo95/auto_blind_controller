#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const int analogInPin = A0;
const int proximityPin = D1;
const int photoPin = D0;
const int motorPinA = D2;
const int motorPinB = D3;

const char *ssid = "K";
const char *password = "20111125";

int brightness = 0;
int distance = 0;
int working_mode = 0;

ESP8266WebServer server(80);
IPAddress local_ip(192, 168, 0, 14);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(115200);

  Serial.print("Connecting to");
  Serial.println(ssid);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(10);

  server.on("/mode_auto", handle_mode_auto);
  server.on("/mode_manual", handle_mode_manual);
  server.on("/manual_up", handle_manual_up);
  server.on("/manual_stop", handle_manual_stop);
  server.on("/manual_down", handle_manual_down);
  server.onNotFound(handle_mode_auto);
  server.begin();
  //
  //  while (WiFi.status() != WL_CONNECTED)
  //  {
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  Serial.println(WiFi.localIP());
  //

  pinMode(proximityPin, OUTPUT);
  pinMode(photoPin, OUTPUT);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(proximityPin, LOW);
  digitalWrite(photoPin, LOW);
  digitalWrite(motorPinA, LOW);
  digitalWrite(motorPinB, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
}

void handle_mode_auto()
{
  working_mode = 0;
  server.send(200, "text/html", SendHTML(0));
}

void handle_mode_manual()
{
  working_mode = 1;
  server.send(200, "text/html", SendHTML(1));
}

void handle_manual_up()
{
  setMotorState(1);
}

void handle_manual_stop()
{
  setMotorState(0);
}

void handle_manual_down()
{
  setMotorState(-1);
}

void setMotorState(int value)
{
  if (value > 0)
  {
    digitalWrite(motorPinA, HIGH);
    digitalWrite(motorPinB, LOW);
  }
  else if (value < 0)
  {
    digitalWrite(motorPinA, LOW);
    digitalWrite(motorPinB, HIGH);
  }
  else
  {
    digitalWrite(motorPinA, LOW);
    digitalWrite(motorPinB, LOW);
  }
}

void loop()
{
  server.handleClient();
  digitalWrite(proximityPin, HIGH);
  delay(100);
  distance = analogRead(analogInPin);
  digitalWrite(proximityPin, LOW);
  delay(100);

  digitalWrite(photoPin, HIGH);
  delay(100);
  brightness = analogRead(analogInPin);
  digitalWrite(photoPin, LOW);
  delay(100);

  Serial.print(brightness);
  Serial.print(" ");
  Serial.println(distance);

  if (working_mode == 0)
  {
    if (brightness > 500)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }

    if (distance > 400)
    {
      setMotorState(1);
      Serial.println("on");
    }
    else
    {
      setMotorState(0);
    }
  }

  delay(1000);
}

String SendHTML(int working_mode)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>LED Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 Web Server</h1>\n";
  ptr += "<h3>Using Access Point(AP) Mode</h3>\n";

  if (working_mode == 0)
  {
    ptr += "<p>Status: Auto</p><a class=\"button button-off\" href=\"mode_auto\">Auto</a>\n";
  }
  else
  {
    ptr += "<p>Staus: Manual</p><a class=\"button button-on\" href=\"/mode_manual\">Manaul</a>\n";
  }

  ptr += "<p>UP</p><a class=\"button button-on\" href=\"/handle_manual_up\">UP</a>\n";
  ptr += "<p>DOWN</p><a class=\"button button-on\" href=\"/handle_manual_down\">DOWN</a>\n";
  ptr += "<p>STOP</p><a class=\"button button-on\" href=\"/handle_manual_stop\">STOP</a>\n";

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
