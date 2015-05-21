/*
  Program: Keypad Door Lock
  Author: Michael Rouse
  Date: 8/2014 - 5/2015
  Description: Will allow for my dorm room door to be unlocked using a keypad
*/
#include <Wire.h>
#include <Keypad.h>
#include <Servo.h> 
#include <EEPROM.h>
#include "MEMORY.h"

// Reset keypad if 15 seconds have passed without input (only after input)
long lastPress = -1;
const int RESET_DELAY = 15000;

// Pin for the buzzer
int buzzerPin = 2;

// Servo Variables
Servo motor;
const int SERVO_PIN = 10;
int pos = 0;

// Password Variables
const int PASSWORD_LENGTH = 15;
char password[PASSWORD_LENGTH];
char typedPassword[PASSWORD_LENGTH];
int position = 0;
String typedPassword = ""; 
bool confirmPassword = false;
bool newPassword = false;
const int PASSWORD_LOCATION = 255;

// Keypad variables
char keyPressed;

const byte rows = 4;
const byte cols = 3;

char keys[rows][cols] = {
						  {'1','2','3'},
						  {'4','5','6'},
						  {'7','8','9'},
						  {'*','0','#'}
						};

byte rowPins[rows] = {11,4,9,8};
byte colPins[cols] = {7,6,5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols); // Keypad object

// Initializer
void setup(){
	// Check to see if it's the programs first time running
	firstTime();
  
  // Read the password from memory
  EEPROM_read(PASSWORD_LOCATION, password);
  
  // Setup Keypad listener for beeps
  keypad.addEventListener(keypadEvent);
  
  // Setup the Servo Motor on Pin #10
  motor.attach(SERVO_PIN);
  motor.write(0);
  delay(500);
  motor.detach();
  
  delay(1000);

}

// Main Loop
void loop(){
  // Get a key from the keypad
  keyPressed = keypad.getKey();
  
	// Check to see if it is time to reset the password (after 15 seconds)
  if (lastPress > -1)
  {
    if (typedPassword != "" || confirmPassword || newPassword)
    {
      if ((millis() - lastPress) >= RESET_DELAY)
      {
        // Timed out- reset everything
        confirmPassword = false;
        newPassword = false;
        reset();
      }
    }
  }
  
  delay(10);
}

// Keypad Event Handler
void keypadEvent(KeypadEvent key)
{
  // Check if a key has been pressed
  if (keypad.getState() == PRESSED)
  {        
    // Perform action based on which key was pressed
    switch (key)
    {
      // Reset Key (*)
      case '*':
        // Check for the new passcode combo
        if (compare(typedPassword, "00000"))
        {
          // Have the user confirm the password
          confirmPassword = true;
          
          Beep(432, 500);
          
          // Clear the passcode
          clear(typedPassword);
          
        }else{
          // Reset the keypad
          reset();
        }

        break;
          
      // Enter Key (#)
      case '#':
        // Check if password is correct
        if (comparePasswords() && !newPassword)
        {
          if (!confirmPassword)
          {
             // Correct password
             accessGranted();
             
          }
          else
          {
            // User password confirmed
            confirmPassword = false;
            
            Beep(800, 300);
            typedPassword = "";
            newPassword = true; // Have the user enter in the new desired password
          }
        
        }
        else
        {
          confirmPassword = false;
          
          if (!newPassword)
          {
            // Incorrect password
            accessDenied();
          }
          else
          {
            newPassword = false;
            
            char newPasscode[15];
            typedPassword.toCharArray(newPasscode, 15);
            
            // Write the password to memory
            EEPROM_write(PASSWORD_LOCATION, newPasscode);
            
            Beep(900, 300);
            Beep(400, 500);
            
            //EEPROM_read(PASSWORD_LOCATION, password);
            // Mark the new password
            for (int i = 0; i < strlen(password); i++)
            {
              password[i] = '1';
            }
            
            EEPROM_read(PASSWORD_LOCATION, password);
            
            for (int i = 0; i < strlen(newPasscode); i++)
            {
              password[i] = newPasscode[i];
            }
            
          }
        }
        
        if (!confirmPassword && !newPassword)
        {
          reset(); // Reset the keypad after it's been unlocked
        }
        
        break;
         
      // Any other key
      default:
        if((typedPassword.length() + 1) < 15)
        {
          // Remember when the last key has been pressed 
          lastPress = millis();
          
          // Give tone feedback
          Beep(860, 100);
          
          // Add to running typed password
          typedPassword = typedPassword + key;
        
        }
        else
        {
          // Password is too long, reset it
          reset();
        }
        
        break;
    }
  }
}

// Unlock the door
void accessGranted()
{
  // Move the servo to 160degrees (this will rotate the door handle enough to unlock it)
  motor.attach(10);
  for(int i = 0; i < 171; i = i + 10){
    motor.write(i);
    delay(50);
  }
  delay(500);
  motor.detach();

  // Beep for 4 seconds
  delay(1000);
  Beep(500, 200);
  delay(1100);
  Beep(500, 200);
  delay(1100);
  Beep(500, 200);
  delay(1100);
  Beep(500, 200);
  delay(300);
  
  // Lock the door
  motor.attach(10);
  for(int i = 170; i > 0; i = i - 10){
    motor.write(i);
    delay(50);
  }
  delay(500);
  motor.detach();

}

// Give Incorrect Password Feedback
void accessDenied()
{
  BeepDelay(300, 300);
  BeepDelay(100, 300);
}

// Reset keypad
void reset()
{
  // Reset the typed password
  typedPassword = "";
  lastPress = -1;

  // Give beep feedback that the door has been reset
  BeepDelay(800, 100);
  BeepDelay(700, 100);
  BeepDelay(500, 100);
 
}

// Make a beep from the buzzer
void Beep(int freq, int duration)
{
  tone(buzzerPin, freq, duration);
}

// Make the buzzer beep, wait until the beep has finished to continue
void BeepDelay(int freq, int duration)
{
  tone(buzzerPin, freq, duration);
  delay(duration);
}

// Compare the passwords
bool comparePasswords()
{
  char typedPass[50];
  bool returnValue = false;
  
  typedPassword.toCharArray(typedPass, 15);
  
  // Start comparing
  if (strlen(typedPass) == strlen(password))
  {
    returnValue = true;
    
    for (int i = 0; i < strlen(password); i++)
    {
      if (password[i] != typedPass[i])
      {
        // Not the same character
        returnValue =  false;
        break;
      }
    }
  }
  
  return returnValue;
}

// compare to char arrays
bool compare(char arr1[], char arr2[])
{
	return (strcmp(arr1, arr2) == 0);
}

// Clears a char array
void clear(char array[])
{
	for(int i = 0; i < sizeof(array); i++)
	{
		array[i] = '';
	}
	return;
}

// Set the default password if it's the first time running
void firstTime()
{
	int x;
	EEPROM_read(0, x);
	
	if (x == -1)
	{
		// Set a default password 
		EEPROM_write(PASSWORD_LOCATION, "12345");
		EEPROM_write(0, 5); // Set the Memory location not equal to -1 so it won't erase the password every time the Arduino restarts
	}
	
	return;
}



/* =================================================
               UPDATE LOG 
====================================================
2015-03-31-Added code so the keypad will reset after 15seconds of no keys being pressed once one has been pressed.
2015-04-01-Fixed bug where the code would reset after every keypress sometimes.
2015-04-12-Added the ability to update the password, it's saved in the Arduino's memory
2014-05-21-Changed the way the program checks to see if there is not a saved password; changed the typed password to a char array


*/

