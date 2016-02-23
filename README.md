<h1>Arduino Keypad</h1>
The Code for my Arduino Keypad

Pictures can be seen on my blog at http://mwrouse.com/blog/9/arduino+keypad+door+lock

<h3>Setting Up</h3>
Move the "MEMORY" folder into your Libraries folder

Move the "Keypad" folder into your Libraries folder


The Libraries folder is located in "My Documents\Arduino"

When you first turn on the Arduino, the password will be set to "12345" However, once you change it the password will remain that even if the keypad looses power and is turned back on.


<h3>Schematic</h3>
![The Schematic for the keypad](https://raw.githubusercontent.com/mwrouse/ArduinoKeypad/master/keypad_schem.jpg)

<h3>Submit a typed password</h3>
1. Press the "#" Key


<h3>Clear the current typed in password</h3>
1. Press the "*" Key or wait 15 seconds and it will clear

 
<h3>Change the password</h3>
1. Enter "00000"
2. Press the "*" key
3. After the tone, enter in your current password (default is "12345")
4. After the tone again enter in your new desired password
5. Password has been changed
