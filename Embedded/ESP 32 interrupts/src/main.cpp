#include <Arduino.h>

#define bt1 2
#define bt2 4
#define bt3 5

struct Button {
  const uint8_t PIN;
  volatile uint32_t numberKeyPresses;
  volatile bool pressed;
};

Button button1 = {bt1, 0, false};
Button button2 = {bt1, 0, false};
Button button3 = {bt1, 0, false};

void isr_bt1() {
  button1.numberKeyPresses += 1;
  button1.pressed = true;
}

void isr_bt2() {
  button2.numberKeyPresses += 1;
  button2.pressed = true;
}

void isr_bt3() {
  button3.numberKeyPresses += 1;
  button3.pressed = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button1.PIN), isr_bt1, FALLING);
}

void loop() {
  if (button1.pressed) {
      Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
      button1.pressed = false;
  }
}