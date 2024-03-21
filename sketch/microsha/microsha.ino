/* Simplified Logitech Extreme 3D Pro Joystick Report Parser */
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include <Servo.h>

Servo bucket;

#define IN1_PIN 3
#define IN2_PIN 4
#define IN3_PIN 7
#define IN4_PIN 8

USB                       Usb;
USBHub                    Hub(&Usb);
HIDUniversal              Hid(&Usb);
JoystickEvents            JoyEvents;
JoystickReportParser      Joy(&JoyEvents);

volatile byte F710Slider = 0x08; // Default value
byte F710SliderLeft = 0x00; // Direction
byte F710SliderRight = 0x00; // Buttons
boolean F710ButtonX = false;
boolean F710ButtonA = false;
boolean F710ButtonB = false;
boolean F710ButtonY = false;

boolean debug = false;
boolean releaseFlag = true;
boolean bucketFlag = true;
boolean moveFlag = false;

void setup() {
  if (debug) {
    Serial.begin( 115200 );
  #if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
    Serial.println("Start");
  }

  if (Usb.Init() == -1)
      debug_print("OSC did not start.");

  delay(200);

  if (!Hid.SetReportParser(0, &Joy))
      if (debug) 
        ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1  );

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  release();

  bucket.attach(5);
  bucket.write(70);

} //setup close

void loop() {
  Usb.Task();

// Direction button
  F710SliderLeft = F710Slider & 0x0F;
  if (F710SliderLeft == 0x08) { // Released all
    if (releaseFlag == false) {
      release();
    }
  }
  else if (F710SliderLeft == 0x00) { // Up
    if (moveFlag == false)
      forward();
  }
  else if (F710SliderLeft == 0x01) { // Up + Right
    debug_print("Up + Right");
  }
  else if (F710SliderLeft == 0x02) { // Right
    if (moveFlag == false)
      right_turn();
  }
  else if (F710SliderLeft == 0x03) { // Down + Right
    debug_print("Down + Right");
  }
  else if (F710SliderLeft == 0x04) { // Down
    if (moveFlag == false)
      backward();
  }
  else if (F710SliderLeft == 0x05) { // Down + Left
    debug_print("Down + Left");
  }
  else if (F710SliderLeft == 0x06) { // Left
    if (moveFlag == false)
      left_turn();
  }
  else if (F710SliderLeft == 0x07) { // Up + Left
    debug_print("Up + Left");
  }


  // A B X Y buttons 
  F710SliderRight = F710Slider & 0xF0;
  if (F710SliderRight == 0x00) { // No buttons
    debug_print("No buttons");
  }
  else {
    F710ButtonX = F710SliderRight & 0x10;
    F710ButtonA = F710SliderRight & 0x20;
    F710ButtonB = F710SliderRight & 0x40;
    F710ButtonY = F710SliderRight & 0x80;

    if (F710ButtonX == true) { // Button X
      debug_print("Button X");
    }
    if (F710ButtonA == true) { // Button A
      debug_print("Button A");
      if (bucketFlag) {
        bucket.write(0);
        bucketFlag = false;
      }
    }
    if (F710ButtonB == true) { // Button B
      debug_print("Button B");
    }
    if (F710ButtonY == true) { // Button Y
      debug_print("Button Y");
      if (!bucketFlag) {
        bucket.write(70);
        bucketFlag = true;
      }
    }
  }

} // loop close

void left_release() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void right_release() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

void left_forward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}

void left_backward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void right_forward() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void right_backward() {
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void forward() {
  left_forward();
  right_forward();
  debug_print("Forward");
  releaseFlag = false;
  moveFlag = true;
}

void backward() {
  left_backward();
  right_backward();
  debug_print("Backward");
  releaseFlag = false;
  moveFlag = true;
}

void release() {
  left_release();
  right_release();
  debug_print("Relese all");
  releaseFlag = true;
  moveFlag = false;  
}

void left_turn() {
  left_backward();
  right_forward();
  debug_print("Left turn");
  releaseFlag = false;
  moveFlag = true;
}

void right_turn() {
  left_forward();
  right_backward();
  debug_print("Right turn");
  releaseFlag = false;
  moveFlag = true;
}

void debug_print(char message[]) {
  if (debug)
    Serial.println(message);
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt)
{
  F710Slider = evt->slider;
}