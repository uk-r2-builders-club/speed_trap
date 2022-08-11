// ---------------------------------------------------------------------------
// This example shows how to use NewPing's ping_timer method which uses the Timer2 interrupt to get the
// ping time. The advantage of using this method over the standard ping method is that it permits a more
// event-driven sketch which allows you to appear to do two things at once. An example would be to ping
// an ultrasonic sensor for a possible collision while at the same time navigating. This allows a
// properly developed sketch to multitask. Be aware that because the ping_timer method uses Timer2,
// other features or libraries that also use Timer2 would be effected. For example, the PWM function on
// pins 3 & 11 on Arduino Uno (pins 9 and 11 on Arduino Mega) and the Tone library. Note, only the PWM
// functionality of the pins is lost (as they use Timer2 to do PWM), the pins are still available to use.
// NOTE: For Teensy/Leonardo (ATmega32U4) the library uses Timer4 instead of Timer2.
// ---------------------------------------------------------------------------
#include <NewPing.h>
#include <LiquidCrystal.h>


#define TRIGGER_PIN_ENTER   8 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN_ENTER      9 // Arduino pin tied to echo pin on ping sensor.
#define TRIGGER_PIN_LEAVE   11
#define ECHO_PIN_LEAVE      10
#define MAX_DISTANCE        50 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define TRAP_DISTANCE       10000 // Distance between sensors in mm
#define READY_LED           13
#define PAUSE_TIME          5000
#define LED_RED              
#define LED_GREEN
#define LED_BLUE

String title = "DBUK - Speed Test";

NewPing sonar_enter(TRIGGER_PIN_ENTER, ECHO_PIN_ENTER, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar_leave(TRIGGER_PIN_LEAVE, ECHO_PIN_LEAVE, MAX_DISTANCE); 


unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
int state = 0;
unsigned long enter_time;
unsigned long leave_time;
bool ready_led = false;

const int rs = 6, en = 7, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  lcd.begin(20,4);
  resetScreen();
  Serial.println("Starting....");
  pinMode(READY_LED, OUTPUT);
  pingTimer = millis(); // Start now.
}

void loop() {
  digitalWrite(READY_LED, ready_led);
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer && state == 0) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar_enter.ping_timer(enterCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    ready_led = true;
  }
  if (millis() >= pingTimer && state == 1) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar_leave.ping_timer(leaveCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  } 
  if (state == 2) {
    Serial.println("Trap done");
    float total_travel_time = leave_time - enter_time;
    Serial.print("Travel time: ");
    Serial.println(total_travel_time);
    float trap_speed = TRAP_DISTANCE/total_travel_time;
    Serial.print("Speed: ");
    Serial.print(trap_speed);
    Serial.print("m/s ");
    Serial.print(trap_speed * 3.6);
    Serial.print("kph ");
    Serial.print(trap_speed * 2.237);
    Serial.println("mph");
    lcd.setCursor(0,2);
    lcd.print(trap_speed);
    lcd.setCursor(4,2);
    lcd.print("m/s");
    lcd.setCursor(0,3);
    lcd.print(trap_speed * 3.6);
    lcd.setCursor(4,3);
    lcd.print("kph");
    lcd.setCursor(10,3);
    lcd.print(trap_speed * 2.237);
    lcd.setCursor(14,3);
    lcd.print("mph");
    delay(PAUSE_TIME);
  }
  // Do other stuff here, really. Think of it as multi-tasking.
}

void enterCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar_enter.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Entered Speed Trap: ");
    state = 1;
    enter_time = millis();
    ready_led = false;
    lcd.setCursor(0,1);
    lcd.print("Running! ");
  }
  // Don't do anything here!
}

void leaveCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar_leave.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Left speed trap: ");
    state = 2;
    leave_time = millis();
    lcd.setCursor(0,1);
    lcd.print("Finished!");
  }
  // Don't do anything here!
}

void resetScreen() {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(title);
    lcd.setCursor(0,1);
    lcd.print("Ready...");
    Serial.println("Screen Reset");
}
