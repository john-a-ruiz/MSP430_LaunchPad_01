# PWM Controller for LED

This schematic is based on the desire to put the chip directly onto a breadboard
so I have incorporated a reset button. If you want to use the LaunchPad ignore the stuff 
coming out of the RST pin.

## 32768 Hz Oscillator
I am using an external oscillator to ensure the accuracy of the specified frequencies.
If you are using the LaunchPad you can solder on the crystal provided or you can stick 
the tuning fork oscillator into the Xin and Xout pins of the LaunchPad (easier)

I used Code Composer Studio (CCS) Version: 6.0.1.00040 on Windows 7. CCS is free and fully supports 
all the M430G2553 functionality.
