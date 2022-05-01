// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
# include "Queue.h"

const int analogInPin = A0;
const int motorPinA = D0;
const int motorPinB = D1;
const int buttonPin = D2;

int brightness = 0;
int bright_sum = 0;
bool buttonState = false;
int working_mode = 0;
long value;
int state = 0;
int cnt = 0;


Queue<int> queue = Queue<int>(5);

void setMotorState(int value)  {
  Serial.print("Motor: ");
  Serial.println(value);
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

bool get_button_state() {
  return not digitalRead(buttonPin);
}

void setup() {
   Serial.begin(115200);
   pinMode(buttonPin, INPUT_PULLUP);
   pinMode(motorPinA, OUTPUT);
   pinMode(motorPinB, OUTPUT);
}

void loop() {
  buttonState = get_button_state();
  queue.push(analogRead(analogInPin));
  brightness = queue.sum() / max(1, queue.count());
  if (queue.count() > 3)
  {
    queue.pop();
  }

  if (cnt % 4 == 0)
  {
    Serial.print(brightness); 
    Serial.print("|");
    Serial.println(buttonState);
    cnt = 0;
  }
  cnt++;
  
  value = Serial.read();

  if (value == '1')
      setMotorState(-1);
  else if (value == '2')
      setMotorState(1);
  else if (value == '3')
      setMotorState(0);

  if (brightness < 380 && state == 0)
  {
    setMotorState(1);
    for (int i; i < 160; i++)
    {
      delay(250);
      if (get_button_state())
        break;      
    }
    setMotorState(0);
    state = 1;
  }
  else if (brightness > 425 && state == 1)
  {
    setMotorState(-1);
    for (int i; i < 128; i++)
    {
      delay(250);
      if (get_button_state())
        break;      
    }
    setMotorState(0);
    state = 0;
  }

  delay(250);  
}
