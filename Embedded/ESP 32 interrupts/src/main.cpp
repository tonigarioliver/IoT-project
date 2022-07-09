#include <Arduino.h>
#define switched true                 // value if the button switch has been pressed
#define triggered true                // controls interrupt handler
#define interrupt_trigger_type RISING // interrupt triggered on a RISING input
#define debounce 10
                   // time to wait in milli secs
#define LED1 13
#define LED2 12
#define bt1 27
#define bt2 26
bool led_status1= LOW; // start with LED off, for testing of switch code only
bool led_status2 = LOW; // start with LED off, for testing of switch code only

volatile bool interrupt_process_status = {
    !triggered // start with no switch press pending, ie false (!triggered)
};
bool initialisation_complete = false; // inhibit any interrupts until initialisation is complete
struct Button
{
  const uint8_t PIN;
  volatile uint32_t numberKeyPresses;
  volatile bool pressed;
};

Button button1 = {bt1, 0, false};
Button button2 = {bt2, 0, false};

/////////////////////ISR FUNCTIONS
void isr_bt1(){
  button1.pressed = true;
}

void isr_bt2()
{
  if (initialisation_complete == true)
  { //  all variables are initialised so we are okay to continue to process this interrupt
    if (interrupt_process_status == !triggered)
    {
      // new interrupt so okay start a new button read process -
      // now need to wait for button release plus debounce period to elapse
      // this will be done in the button_read function
      if (digitalRead(button2.PIN) == HIGH)
      {
        // button pressed, so we can start the read on/off + debounce cycle wich will
        // be completed by the button_read() function.
        interrupt_process_status = triggered; // keep this ISR 'quiet' until button read fully completed
      }
    }
  }
} // end of button_interrupt_handler

bool read_button()
{
  int button_reading;
  // static variables because we need to retain old values between function calls
  static bool switching_pending = false;
  static long int elapse_timer;
  if (interrupt_process_status == triggered)
  {
    // interrupt has been raised on this button so now need to complete
    // the button read process, ie wait until it has been released
    // and debounce time elapsed
    button_reading = digitalRead(button2.PIN);
    if (button_reading == HIGH)
    {
      // switch is pressed, so start/restart wait for button relealse, plus end of debounce process
      switching_pending = true;
      elapse_timer = millis(); // start elapse timing for debounce checking
    }
    if (switching_pending && button_reading == LOW)
    {
      // switch was pressed, now released, so check if debounce time elapsed
      if (millis() - elapse_timer >= debounce)
      {
        // dounce time elapsed, so switch press cycle complete
        switching_pending = false;             // reset for next button press interrupt cycle
        interrupt_process_status = !triggered; // reopen ISR for business now button on/off/debounce cycle complete
        return switched;                       // advise that switch has been pressed
      }
    }
  }
  return !switched; // either no press request or debounce period not elapsed
}
////////////////////////////////Other function////////////////////////////////
void pinSETUP(){
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(button2.PIN, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(button1.PIN), isr_bt1, interrupt_trigger_type);
  attachInterrupt(digitalPinToInterrupt(button2.PIN), isr_bt2, interrupt_trigger_type);
  initialisation_complete = true; // open interrupt processing for business
}

///////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  void pinSETUP();
}

void loop()
{
  ////////////BUTTON 1
  if (button1.pressed)
  {
    uint8_t read = LOW;
    static long elapse_timer;
    static uint8_t lastState;
    if(digitalRead(button1.PIN) == HIGH){
       read = HIGH;
    }
    if(millis() - elapse_timer >=debounce){
        if(lastState != read && lastState ==LOW){
          Serial.println("Changed digital");
          led_status1 = HIGH - led_status1; // toggle state
          digitalWrite(LED1, led_status1);
        }
        elapse_timer = millis();
        lastState = read;
    }
    button1.pressed =false;
  }

  // BUTTON 2
  if (read_button() == switched)
  {
    // button on/off cycle now complete, so flip LED between HIGH and LOW
    led_status2 = HIGH - led_status2; // toggle state
    digitalWrite(LED2, led_status2);
  }
  else
  {
    // do other things....
  }
}