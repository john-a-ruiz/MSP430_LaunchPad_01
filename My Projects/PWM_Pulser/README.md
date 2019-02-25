# PWM Controller for LED

### !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
### I have added a few define's to the msp430g2553.h file. Nothing major.
### In most cases they are define TERM 0x0000; so when you compile the code YOU WILL GET ERRORS.
### To make the code work you are going to have to do a little bit of thinking but not too much. 
### If you need any help let me know :)
### !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

This schematic is based on the desire to put the chip directly onto a breadboard
so I have incorporated a reset button. If you want to use the LaunchPad ignore the stuff 
coming out of the RST pin.

I used Code Composer Studio (CCS) Version: 6.0.1.00040 on Windows 7. CCS is free and fully supports 
all the M430G2553 functionality.

## 32768 Hz Oscillator
I am using an external oscillator to ensure the accuracy of the specified frequencies.
If you are using the LaunchPad you can solder on the crystal provided or you can stick 
the tuning fork oscillator into the Xin and Xout pins of the LaunchPad (easier)

## YouTube Video
[![MSP430 Launchpad Project (code and schematics): PWM contoller on a Breadboard](http://img.youtube.com/vi/U2Fl7Byrlf4/0.jpg)](https://www.youtube.com/watch?v=U2Fl7Byrlf4)
