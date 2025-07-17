// Welcome to my Cybertruck Model R Mini Code

/*  This is the key for the hexademical values of the different remote control keys
//Ch- = FFA25D
CH = FF629D
CH+ = FFE21D
backwards = FF22DD
forwards = FF02FD
pause = FFC23D
minus = FFE01F
plus = FFA857
EQ = FF906F
0 = FF6897
100+ = FF9867 
200+ = FFB04F
1 = FF30CF
2 = FF18E7
3 = FF7A85
4 = FF10EF
5 = FF38C7
6 = FF5AA5
7 = FF42BD or FFFFFFFF
8 = FF4AB5
9 = FF52AD
*/



#include <IRremote.h> // import infrared remote library (Version 2.2.3)
#include <Wire.h> // import wire library
#include <LiquidCrystal_I2C.h> // import library for LCD keypad shield

LiquidCrystal_I2C lcd(0x27, 16, 2); // define the type of I2C lcd keypad shield (mine is 0x27), the number of characters, and the number of rows

const int IRpin = 2; // define pin for remote control

// create an instance for the receiving signal and create an instance to store the collected data
IRrecv IR(IRpin);
decode_results results;

// define pins for ultrasonic sensor connection
const int triggerPin = 7;
const int echoPin = 8;

// define long integers to store values for the time and distance (using time, we divide by 58.2 to get the distance)
long Time;
int distance = 0;


// define pins for dc motor connection
const int leftMotor_forward = 9;
const int leftMotor_backward = 10;
const int rightMotor_forward = 11;
const int rightMotor_backward = 12;

//define pins for LED connection
const int LED1 = 1;
const int LED2 = 3;
const int LED3 = 4;
const int LED4 = 5;
const int LED5 = 6;
const int LED6 = 13;

// initialize
void setup() {
  // start the LCD display
  lcd.init();
  // turn on the light of the display
  lcd.backlight();

  // set trigger pin as output (to emit waves) and echo pin as input (to receive waves) for ultrasonic sensor
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // set DC motor pins to output
  pinMode(leftMotor_forward, OUTPUT);
  pinMode(leftMotor_backward, OUTPUT);
  pinMode(rightMotor_forward, OUTPUT);
  pinMode(rightMotor_backward, OUTPUT);

  // set LEDs as output
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);

  // activate the IR receiver to detect signals emitted from the remote when keys are pressed
  IR.enableIRIn();

  // begin serial communications at 115200 baud rate
  Serial.begin(115200);

  // by default, the set mode is manual mode so manual mode is printed on the first row
  lcd.print("Manual Mode");

  // move the cursor down to the second row
  lcd.setCursor(0,1);
}

void loop() {
   // this activates the triggerPin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);   
  digitalWrite(triggerPin, HIGH);     // send waves for 10 us
  delayMicroseconds(10);

  // the time it takes for the waves to return back to the sensor is determined using the echoPin which takes input
  Time = pulseIn(echoPin, HIGH); 

  // using the time taken, divide that by 58.2 to get the distance
  distance = Time / 58.2;  
  delay(10);

  // print the distance onto the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // tell IR receiver to decode a signal (if it receives any). If it doesn't receive a signal, just wait
  while (IR.decode(&results) == 0) {
    }
    
  // if a signal is received, then print the hexademical value (this information is needed later for pressing buttons on the remote, and then making the cybertruck perform actions)
  Serial.println(results.value, HEX);
  delay(1500);

  // resume detecting for signals 
  IR.resume();

  // the manual mode function is called
  manualMode();

  // if the "EQ" button is pressed on the remote, enter autopilot mode 
  
  if (results.value == 0xFF906F) {
    // if there is no nearby object, move forward
    if (distance > 19)            
    {
      forward();                                                       
    }

    // if a nearby object is detected, perform the necessary actions in order to avoid hitting the object
    if (distance < 18)
    {
      Stop();
      delay(500);
      backward();
      delay(500);
      Stop(); 
      delay(100);  
      turnLeft(); 
      delay(500);
      Stop();
      delay(500);
      forward();
      delay(500);
    }
}
}

// THESE NEXT 5 METHODS DEFINED CONTROL THE MOVEMENT OF THE MOTOR USING THE L293D DRIVER CHIP. NOTE THAT MESSAGES ARE ONLY PRINTED ON THE DISPLAY IF IT IS NOT ON AUTOPILOT MODE.

// this method defines how the robot moves forward by activating both motors and telling them to move forward
void forward() {
  digitalWrite(leftMotor_forward, HIGH);
  digitalWrite(leftMotor_backward, LOW);
  digitalWrite(rightMotor_forward, HIGH);
  digitalWrite(rightMotor_backward, LOW);
   if (results.value != 0xFF906F) {
      // print the action onto the display
      lcd.print("Moving Forward");
    }
  delay(1000);
}

// this method defines how the robot stops by deactivating both motors
void Stop() {
    digitalWrite(leftMotor_forward, LOW);
    digitalWrite(leftMotor_backward, LOW);
    digitalWrite(rightMotor_forward, LOW);
    digitalWrite(rightMotor_backward, LOW);
    // turn on the back lights to indicate that the vehicle stopped
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      delay(i * 1000);
      }
    // after some time they are on, turn them off
    turnOffLights();
    if (results.value != 0xFF906F) {
      // print the action onto the display
      lcd.print("Stopped");
    }
    delay(1000);
  
}

// this method defines how the robot moves backward by activating both motors and telling them to move backwards
void backward() {
  digitalWrite(leftMotor_forward, LOW);
  digitalWrite(leftMotor_backward, HIGH);
  digitalWrite(rightMotor_forward, LOW);
  digitalWrite(rightMotor_backward, HIGH);
  
  // turn on the back lights to signal the reversing motion to other cars
  for (int i = 0; i < 3; i++) {
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      delay(i * 1000);
      }
  // turn off the lights after some time they are on
  turnOffLights();
   if (results.value != 0xFF906F) {
      // print the action onto the display
      lcd.print("Moving backward");
    }
  delay(1000);
}

// this method defines how the robot turns left by making the right motor move forward, and the left motor move backward
void turnLeft() {
  digitalWrite(leftMotor_forward, LOW);
  digitalWrite(leftMotor_backward, HIGH);
  digitalWrite(rightMotor_forward, HIGH);
  digitalWrite(rightMotor_backward, LOW);
  // make sure the left side LED blinks when turning left
   for (int i = 0; i < 3; i++) {
      digitalWrite(LED3, HIGH);
      delay(i * 1000);
      digitalWrite(LED3, LOW);
      delay(i * 1000);
      }
  // turn off the lights after it blinks for some time
  turnOffLights();
   if (results.value != 0xFF906F) {
      lcd.print("Turning Left");
    }
  delay(1000);
}

// this method defines how the robot turns right by making the left motor move forward, and the right motor move backward
void turnRight() {
  digitalWrite(leftMotor_forward, HIGH);
  digitalWrite(leftMotor_backward, LOW);
  digitalWrite(rightMotor_forward, LOW);
  digitalWrite(rightMotor_backward, HIGH);
  // make sure the right side LED blinks when turning right
  for (int i = 0; i < 3; i++) {
      digitalWrite(LED4, HIGH);
      delay(i * 1000);
      digitalWrite(LED4, LOW);
      delay(i * 1000);
      }
  turnOffLights();
   if (results.value != 0xFF906F) {
      // print the message onto the display
      lcd.print("Turning right");
    }
  delay(1000);
}

// this method defined makes sure to turn off the side lights and the back lights
void turnOffLights() {
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
  digitalWrite(LED6, LOW);
  }

// this method allows you to turn on or off the headlights manually
void headlights() {
  if (results.value == 0xFFE21D) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    lcd.print("Headlights on");
    }

  if (results.value == 0xFFA25D) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    lcd.print("Headlights Off");
    }
  }

// this method is extremely important since it tells the cybertruck what function to perform if a specific key is pressed on the remote by the user
void manualMode() {
  lcd.print("   Manual Mode");
  lcd.setCursor(0, 1);
  if (results.value == 0xFF02FD) {
    forward();
    }
  if (results.value == 0xFF22DD) {
    backward();  
    }
  if (results.value == 0xFFC23D) {
    Stop();
    }
  if (results.value == 0xFFE01F) {
    turnLeft();
    }
  if (results.value == 0xFFA857) {
    turnRight();
    }

  if (results.value == 0xFFE21D || results.value == 0xFFA25D) {
    headlights();
    }
  lcd.clear();
  }
