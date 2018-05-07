Overview
--------------------
* Name : Cap_meter_arduino
* Title : Capacitance meter for Arduino
* Description : Capacitance meter for Arduino, two tests , range 18pf to 4F , Push Button input , OLED and serial monitor output.
* Author: Gavin Lyons


Libraries
------------------------

The .ino file( which contains the C++ source code) calls from a number of libraries.

All are standard libraries which can be found on Arduino website site or Adafruit website

* <Wire.h>   //I2C 
* <Adafruit_GFX.h> // OLED 
* <Adafruit_SSD1306.h> // OLED
* <Button.h> // push button https://www.arduinolibraries.info/authors/madleech https://github.com/madleech/Button


Parts List
------------------------------
See fritzing diagram in "doc" for assembly instruction and schematic. 
You will need [fritzing](https://en.wikipedia.org/wiki/Fritzing) software to open this.

You will need following parts

>Two pushbuttons
>
>Two resistors 10kohm and 220ohm 
>
>Arduino microcontroller 
>
> I2C 0.91" inch 128x32 OLED Display Module 


Features
-----------------------------------------------
The meter measures capacitance it outputs to an OLED and serial monitor.
Two push buttons start the two tests.
Each Arduino capacitance meter relies on a property of resistor capacitor (RC) circuits- the time constant. 
The time constant of an RC circuit is defined as the time it takes for the voltage across the capacitor 
to reach 63.2% of its voltage when fully charged:

![ScreenShot cap time constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/docs/ctc.jpg)

Larger capacitors take longer to charge, and therefore will create larger time constants. 
The capacitance in an RC circuit is related to the time constant by the equation:

![ScreenShot cap eq constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/docs/eq.jpg)

The capacitance meter has an RC circuit with known resistor values and an unknown capacitor value. 
The Arduino will measure the voltage at the capacitor and record the time it takes to reach 63.2% of it’s voltage when fully charged (the time constant). 
Since the resistance value is already known, 
we can use the formula above in a program that will calculate the unknown capacitance.

Test 1
###
Range  1 uF to 4F. 
Uses two digital pins, one analog pin, and two resistors.
One for discharge one for charge
Insert a capacitor in range into terminal press button and view result on OLED or serial monitor
The result gives two values value of cap in uF and time constant to test in mS.

![ScreenShot cap eq constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/docs/sch.jpg)

Test2
###
Test 2 : Range 18 pF to 470 uF.
This capacitance test has the greatest range of two. It also had the highest accuracy with smaller capacitors. 
No resistors are needed and it only uses two analog pins(A2(neg) and A3(positive) from the Arduino.
There is no extra resistor just the internal components of atmega chip.
The internal analog pin capacitor to gnd (C1) varies between 20 and 30pF. unknown capacitor = Cu.
The positive pin is set to 5V, the negative to 0V.
VA2 = (VA3*Cu)/(C1 + Cu).

The value of C1 for any given arduino board wil have to be calibarted using a known cap for Cu
for full accuracy I set it at 24.48pF in code(IN_STRAY_CAP_TO_GND) Found using equation.
C1 = Cu *(VA3-VA2) / VA3, Where  Va2 is ADC value reported by code and VA3 is 5v or 1023.
so for a known capacitor of 103pF giving a ADC value of 801
so C1 = 102pf * (1023-801)/ 1023 = 28.48pF.
The user will have to change IN_STRAY_CAP_TO_GND for their system.

Test a known cap get Adc value and then pop these values into equation above
and put the C1 value into IN_STRAY_CAP_TO_GND  varible in code.
The test displays 3 values 
* Capacitnace in Farads
* Time constant for test in mS
* ADC measurement from 0 to 1023 where 1023 is 5V

![ScreenShot cap eq constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/docs/test2sch.jpg)


See Also
-----------------------------------

* https://www.arduino.cc/en/Tutorial/CapacitanceMeter
* http://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/
* http://electronoobs.com/eng_arduino_tut10_1.php
* http://wordpress.codewrite.co.uk/pic/2014/01/21/cap-meter-with-arduino-uno/

Copyright
-------------------------------

Copyright (C) 2018 G Lyons. This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, see license.md for more details
