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


// Variables for auto-resetting the keypad
const int RESET_DELAY = 15000; // Time until the keypad is reset after the last press (in milliseconds)
long lastPress = -1;

// Buzzer variables
const int BUZZER = 2;

// Servo Variables
const int SERVO = 10; // Pin the servo is on
int pos = 0; // Current position of the servo
Servo motor;

// Password Variables
const int PASSWORD_LOCATION = 255;
const int PASSWORD_LENGTH = 15;

char password[PASSWORD_LENGTH];
char typedPassword[PASSWORD_LENGTH];
int passPosition = 0;

bool confirmPassword = false;
bool newPassword = false;

// Keypad variables
const byte ROWS = 4;
const byte COLS = 3;

char keyPressed; // Variable for the last key to be pressed

// Map of the keypad layout
char keys[ROWS][COLS] = {
						  {'1','2','3'},
						  {'4','5','6'},
						  {'7','8','9'},
						  {'*','0','#'}
						};

// Pins that each keypad row and column is connected to
byte rowPins[ROWS] = {11, 4, 9, 8};
byte colPins[COLS] = {7, 6, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Keypad object


// Initializer
void setup(){
  // Check to see if it's the programs first time running
  firstTime();
  
  // Read the password from memory
  EEPROM_read(PASSWORD_LOCATION, password);
  
  // Setup Keypad listener for beeps
  keypad.addEventListener(keypadEvent);
  
  // Setup the Servo Motor on Pin #10
  motor.attach(SERVO);
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
    // Make sure that a password has been typed
    if (!compare(typedPassword, ""))// || confirmPassword || newPassword)
    {
      // Check to see if the RESET_DELAY has been reached
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
          // User wants to enter a new password, start this process
          // Have the user confirm the password
          confirmPassword = true;
          
          Beep(432, 500);
          
          // Clear the passcode
          empty(typedPassword);
          
        }else{
          // Reset the keypad
          reset();
        }

        break;
          
      // Enter Key (#)
      case '#':
        // Check if password is correct and the user is not typing in a new password
        if (compare(typedPassword, password) && !newPassword)
        {
          // Check if the user is trying to confirm the password to change it
          if (confirmPassword)
          {
            // User password confirmed
            confirmPassword = false;
            
            Beep(800, 300);
            empty(typedPassword); // Empty the typed password var
            newPassword = true; // Have the user enter in the new desired password
             
          }
          else
          {
            // Correct password
             accessGranted();        
          }
        
        }
        else
        {
          // Password was wrong or the user is trying to enter a new password
          confirmPassword = false;
          
          // Deny access if not setting a new password
          if (!newPassword)
          {
            // Incorrect password
            accessDenied();
          }
          else
          {
            // Set the new password
            newPassword = false;
            
            // Write the password to memory
            EEPROM_write(PASSWORD_LOCATION, typedPassword);
            
            Beep(900, 300);
            Beep(400, 500);
            
            //EEPROM_read(PASSWORD_LOCATION, password);
            // Mark the new password
            for (int i = 0; i < strlen(password); i++)
            {
              password[i] = '1';
            }
            
            EEPROM_read(PASSWORD_LOCATION, password);
            
            for (int i = 0; i < strlen(typedPassword); i++)
            {
              password[i] = typedPassword[i];
            }
            
          }
        }
        
        // Reset the keypad after submiting a password unless the user is trying to change the password
        if (!confirmPassword && !newPassword)
        {
          reset(); // Reset the keypad after it's been unlocked
        }
        
        break;
         
      // Any other key (a digit key)
      default:
        // Make sure that the password isn't too long
        if((passPosition + 1) < PASSWORD_LENGTH)
        {
          // Remember when the last key has been pressed (for resetting the keypad automatically) 
          lastPress = millis();
          
          // Give tone feedback
          Beep(860, 100);
          
          // Add to running typed password
          typedPassword[passPosition] = key;
          
          passPosition += 1;
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
  empty(typedPassword);
  passPosition = 0;
  lastPress = -1;

  // Give beep feedback that the door has been reset
  BeepDelay(800, 100);
  BeepDelay(700, 100);
  BeepDelay(500, 100);
 
}

// Make a beep from the buzzer
void Beep(int freq, int duration)
{
  tone(BUZZER, freq, duration);
}

// Make the buzzer beep, wait until the beep has finished to continue
void BeepDelay(int freq, int duration)
{
  tone(BUZZER, freq, duration);
  delay(duration);
}


// compare to char arrays
bool compare(char arr1[], char arr2[])
{
	return (strcmp(arr1, arr2) == 0);
}

// Clears a char array
void empty(char array[])
{
	for(int i = 0; i < sizeof(array); i++)
	{
		array[i] = '\0';
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

