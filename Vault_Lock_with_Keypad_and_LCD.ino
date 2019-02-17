/* Libraries */
#include <Servo.h> // For Servo
#include <Wire.h> // For I2C
#include <LiquidCrystal_I2C.h> // For LCD
#include <Keypad.h> // For Keypad

Servo door_servo; // Create Servo object to control the Servo

const int servoPin = 9;    // The pin that the servo is attached to
const int greenLEDPin = 10;    // The pin that the Green LED is attached to
const int redLEDPin = 11;    // The pin that the Red LED is attached to
const int buzzer = 12; // The pin of the active buzzer
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char hexaKeys[ROWS][COLS] = // Define the cymbols on the buttons of the keypads
{
  { '1', '2', '3', '4' },
  { '5', '6', '7', '8' },
  { '9', 'A', 'B', 'C' },
  { 'D', '*', '0', '#' }
};
byte colPins[COLS] = { 4, 5, 6, 7 }; // Connect to the row pinouts of the keypad
byte rowPins[ROWS] = { 3, 2, 1, 0 }; // Connect to the column pinouts of the keypad

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display

int pos = 0;
int correct = 0;
bool doorOpened = false;

// Initial Secret Code
char secretCode[6] = { '1', '2', '3', '4', '5', '6' };
char inputCode[6] = { '0', '0', '0', '0', '0', '0' };

// Initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // Door Servo
  door_servo.attach(servoPin); // Attachs the Servo to Servo object
  door_servo.write(0); // Back to 0 degrees

  // Buzzer
  pinMode(buzzer, OUTPUT); // Initialize the buzzer pin as an output

  // LED lights
  pinMode(greenLEDPin, OUTPUT); // Initialize the led pin as output
  pinMode(redLEDPin, OUTPUT); // Initialize the led pin as output
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);

  // LCD
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn the backlight
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");
  delay(1000);
}


// Reset
void(* resetFunc) (void) = 0;

void loop() {
  readKey();
}

void readKey() {
  int i;
  char customKey = customKeypad.getKey(); // Get the key value

  if (customKey) {
    switch (customKey) {
      case 'D':
        lock();
        break;
      case '*': // If press in "*", then let user enter Passcode
        if (!doorOpened) {
          pos = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Input Your Code:");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Door is open.");
          lcd.setCursor(0, 1);
          lcd.print("Please reset.");
        }
        break;
      case '#':  // If press in "#", then check if the password is right
        for (i = 0; i < 6; i++) {
          if (inputCode[i] == secretCode[i]) {
            correct ++;
          }
        }

        // If code is right
        if (correct == 6) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted!"); // Print success
          digitalWrite(greenLEDPin, HIGH);
          buzz();
          delay(200);
          
          // Turn Servo
          door_servo.write(90); // Goes to 15 degrees
          
          doorOpened = true;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Denied!"); // Access Denied
          digitalWrite(redLEDPin, HIGH);
          buzz();
          delay(300)
          resetFunc();
        }
        break;
      default:
        if (!doorOpened) {
          inputCode[pos] = customKey;
          lcd.setCursor(pos, 1);
          lcd.print(inputCode[pos]);
          pos ++;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Door is still opened.");
          lcd.setCursor(0, 1);
          lcd.print("Press reset.");
        }
    }
  }
}

// Buzzer
void buzz() {
  digitalWrite(buzzer, HIGH);
  delay(500); // Wait for 1ms
  digitalWrite(buzzer, LOW);
}

void lock() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Locked!");
  digitalWrite(buzzer, HIGH);
  delay(100); // Wait for 100ms
  digitalWrite(buzzer, LOW);
  delay(100); // Wait for 100ms
  digitalWrite(buzzer, HIGH);
  delay(100); // Wait for 100ms
  digitalWrite(buzzer, LOW);
  delay(300);
  resetFunc();
}
