# ArduinoKeypad
The Code for my Arduino Keypad

Setting Up:
Move the "MEMORY" folder into your Libraries folder for the Arduino
My Documents\Arduino\libraries

Once you upload the code to your Arduino, you need to comment out the line
"#define DEFAULT_PASSWORD" or everytime the Arduino resets the password will change to "12345"

# To submit a typed password
1. Press the "#" Key


# To clear the current typed in password
1. Press the "*" Key or wait 15 seconds and it will clear

 
# To change the passcode
1. Enter "00000"
2. Press the "*" key
3. After the tone, enter in your current password (default is "12345")
4. After the tone again enter in your new desired password
5. Password has been changed
