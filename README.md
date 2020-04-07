Table of contents
---------------------------

  * [Table of contents](#table-of-contents)
  * [Overview](#overview)
  * [Libraries](#libraries)
  * [Software used](#software-used)
  * [Parts Parts](#parts-lits)
  * [Features](#features)
  * [Project Schematic](#project-schematic)
  * [See  Also](#see-also)
  
Overview
--------------------
* Name : Cap_meter_arduino
* Title : Capacitance meter Arduino based microcontroller.
* Description : Capacitance meter for Arduino, three tests , 
range 18pf to 4F , Push Button input , OLED, serial monitor output and LED(onboard NANO)
* Author: Gavin Lyons
* URL: https://github.com/gavinlyonsrepo/Cap_meter_arduino
* License: Copyright (C) 2018 G Lyons. See license.md for more details.

Libraries
------------------------

The .ino file( which contains the C++ source code) calls from a number of libraries.

All are standard libraries which can be found on Arduino website site or Adafruit website

* <Wire.h>   v 1.0.0 //I2C 
* <Adafruit_SSD1306.h> v 1.1.2 // OLED
* <Button.h> v 1.0.0 // pb https://www.arduinolibraries.info/authors/madleech or https://github.com/madleech/Button

Software used
-----------------------------
Eagle 9.1.3

Arduino 1.8.5

Parts List
------------------------------
Eagle schematic diagram and image in "docs". 

You will need following parts

> Three pushbuttons
>
> Resistors: Test2(10Kohm and 220ohm) , Test3(10K, 3.1K, 1.8K)
>
> Arduino NANO micro-controller module (tested on a NANO and UNO)
>
> I2C 0.91" inch 128x32 OLED Display Module SSD1306 Driver IC
>
> Some sort of terminals or socket to hold or connect to capacitors during test
>

Features
-----------------------------------------------

The meter measures capacitance it outputs to an OLED and serial monitor.
(The OLED is optional as data via serial monitor on a PC)
Three push buttons start the three tests respectively.

1. Test 1 Range  1 uF to 4F.
2. Test 2 Range 18 pF to 470 uF.
3. Test 3 Range 4.7 nF to 180 uF.

The on-board LED of nano module D13 is used as Status LED.
It Flashes during startup, stays on during "READY to test" state and goes off during "TESTING" state

The OLED displays  brief help message and title screen duirng start-up

Each Arduino capacitance meter relies on a property of resistor capacitor (RC) circuits- the time constant. 
The time constant of an RC circuit is defined as the time it takes for the voltage across the capacitor 
to reach 63.2% of its voltage when fully charged:

![ScreenShot cap time constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/images/ctc.jpg)

Larger capacitors take longer to charge, and therefore will create larger time constants. 
The capacitance in an RC circuit is related to the time constant by the equation:

![ScreenShot cap eq constant](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/images/eq.jpg)

The capacitance meter has an RC circuit with known resistor values and an unknown capacitor value. 
The Arduino will measure the voltage at the capacitor and record the time it takes to reach 63.2% of it’s voltage when fully charged (the time constant). 
Since the resistance value is already known, 
we can use the formula above in a program that will calculate the unknown capacitance.

**Test 1**


Test 1: Range  1 uF to 4F. 
Uses two digital pins, one analog pin, and two resistors.
One for discharge, one for charge.
Insert a capacitor in range into terminal press button and view result on OLED or serial monitor.
The Capacitor is charged,  We can measure elapsed time until time constant fulfilled using analog pin and we know R 
value. Therefore we solve for C. 
The result gives two values: Capacitance value and time constant to test in mS.
The user can adjust the 10K resistor value in code variable(resistorValue) to match exact value, to improve accuracy

![ScreenShot cap sch](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/images/sch.jpg)

**Test 2**

Test 2 : Range 18 pF to 470 uF.

This capacitance test has the greatest range of 3. It also had the highest accuracy with smaller capacitors. 
No resistors are needed and it only uses two analog pins(A2(neg) and A3(positive) from the Arduino.
There is no extra resistor just the internal components of ATmega328 chip.

C1 calibration(optional): 

The internal analog pin capacitor to gnd (C1) varies between 20 and 30pF. unknown capacitor = Cu.
The positive pin is set to 5V, the negative to 0V.
VA2 = (VA3 * Cu)/(C1 + Cu).
The value of C1 for any given Arduino board will have to be calibrated using a known cap for Cu,
for full accuracy I set it at 24.48pF in code (CapOne) Found using equation.
C1 = Cu * (VA3-VA2) / VA3, Where  Va2 is ADC value reported by code and VA3 is 5v or 1023.
So for a known capacitor of 103pF giving a ADC value of 801.
So C1 = 103pf * (1023-801)/ 1023 = 28.48pF.
The user should change this for their system to improve accuracy.
Test a known cap get ADC value and then pop these values into equation above
and put the C1 value into (CapOne) variable in code.

Test equation:

Cu = VA2 * C1 / (VA3 - VA2)

The test displays 3 values 
* Cu,  Capacitance in Farads 
* Time constant for test in mS
* Va2 , ADC measurement from 0 to 1023 where 1023 is 5V 

![ScreenShot cap sch 2](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/images/test2sch.jpg)

**Test 3** 


Test 3 Range 0.0047 uF to 180 uF.

![ScreenShot cap sch 3](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/images/test3sch.jpg)

A voltage divider is created to give a reference voltage,

Vout = Vin * (R5 / (R5 + R4)) , 5 * (3100 / (1800 + 3100)) = 3.163.

The objective here is to find the time constant τ (tau) in the equation:

τ = R * C Solved for C: C = τ / R

So we want to charge the capacitor under test with a suitable voltage (eg. 5V from an Arduino output pin) and measure how long it takes to reach 63.2% of that voltage. So we need a reference voltage of 5 * 0.632, namely 3.16V.
The internal analogue comparator is used for this job.:

1. pin 6 PD6 AIN0
2. pin 7 PD7 AIN1
 
We connect the reference voltage to the AIN1 pin (negative reference) and connect our capacitor to the AIN0 pin (positive reference) and then configure an interrupt on the rising edge. A value of 10K for the resistor to give a reasonably slow charge time. The value of ~ 10K is specified in code variable (resistance).
so adjust for your resistor value , I have 9830 ohms. Make sure to pick accurate resistors for voltage divider.

The test returns the Capacitance result and time constant.

Project Schematic 
----------------------------

![ScreenShot cap sch 3](https://github.com/gavinlyonsrepo/Cap_meter_arduino/blob/master/docs/eagle/cap_meter.png)

See Also
-----------------------------------
 
* https://www.arduino.cc/en/Tutorial/CapacitanceMeter
* http://www.circuitbasics.com/how-to-make-an-arduino-capacitance-meter/
* http://electronoobs.com/eng_arduino_tut10_1.php
* http://wordpress.codewrite.co.uk/pic/2014/01/21/cap-meter-with-arduino-uno/
* http://www.gammon.com.au/forum/?id=12075
