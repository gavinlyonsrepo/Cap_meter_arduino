//******************* HEADER ***********************************
/*
  Name : Cap_meter_ardunio
  Title : Capacitance meter for Ardunio
  Description : Capacitance meter for Ardunio, three tests , range 10pf to 4F , 
  Push Button input , OLED and serial monitor output.
  Author: Gavin Lyons
  URL: https://github.com/gavinlyonsrepo/Cap_meter_arduino
*/

//*************************** LIBRARIES ********************
#include <Wire.h>   //I2C 1.0.0
#include <Adafruit_SSD1306.h> // OLED 1.1.2
#include <Button.h> // push buttons 1.0.0

//*************************** GLOBALS ********************

// LCD data
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


// Pins and vars for test 1 
#define analogPin      6
#define chargePin      12
#define dischargePin   11
#define resistorValue  10000.0F


// Pins and vars for test 2
const int OUT_PIN = A3;  // pos pin
const int IN_PIN = A2;
const float IN_STRAY_CAP_TO_GND = 24.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;
const int MAX_ADC_VALUE = 1023;


//Pins and vars for test 3
const byte pulsePin = 2;
const unsigned long resistance = 10000;
volatile boolean triggered;
volatile boolean active;
volatile unsigned long startTime;
volatile unsigned long duration;

//Interupt service rountine
ISR (ANALOG_COMP_vect)
  {
  unsigned long now = micros ();
  if (active)
    {
    duration = now - startTime;
    triggered = true;
    digitalWrite (pulsePin, LOW); 
    }
  }
  
// Buttons and test count
Button btn_test(3);
Button btn_test_two(8);
Button btn_test_three(10);
int test_count = 0;

//*************************** SETUP ************************
void setup() {

  // Setup pins for button enable internal pull-up resistors
  digitalWrite(3, HIGH);
  digitalWrite(8, HIGH);
   digitalWrite(10, HIGH);
  btn_test.begin();
  btn_test_two.begin();
  btn_test_three.begin();

  // Setup pins for testing 1 and 2
  pinMode(chargePin, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(IN_PIN, OUTPUT);
  digitalWrite(chargePin, LOW);
  //test3
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);
  ADCSRB = 0;
  ACSR =  _BV (ACI)
          | _BV (ACIE)
          | _BV (ACIS0) | _BV (ACIS1);
          
  // Setup serial
  Serial.begin(9600);
  //display intial OLED screen
  Display_init();
  Serial.println("------------- CAP Meter Comms UP ------------");
  delay(500);
}

//******************* MAIN LOOP *****************
void loop() {
  // *** TEST 1 Range 1 uF to 4F. ***
  if (btn_test.pressed()) {
    TestButton(1);
    Test_one();
  }
  // *** TEST 2  Range 18 pF to 470 uF. ***
  if (btn_test_two.pressed()) {
    TestButton(2);
    Test_two();
  }
   // *** Test 3 Range 0.0047 uF to 180 uF. ***
  if (btn_test_three.pressed()) {
    TestButton(3);
    Test_three();
  }
}


// ********************* FUNCTIONS *************************

//Function to handle test1
void Test_one()
{
  // carry out Test
  unsigned long startTime;
  unsigned long elapsedTime;
  float uF;
  float nF;
  digitalWrite(chargePin, HIGH);
  startTime = millis();
  // 648 is 63.2% of 1024 adc 10 bit = 1024 stop when get to 648
  while (analogRead(analogPin) < 648) {
  }
  elapsedTime = millis() - startTime;

  // Calculate and display value, c = TC/R
  uF = ((float)elapsedTime / resistorValue) * 1000;
  Display_time(elapsedTime);
  if (uF > 1) {
    Serial.print((long)uF);
    Serial.println(" microFarads");
    display.print((long)uF);
    display.println(" uF");
  }
  else {
    nF = uF * 1000.0;
    Serial.print((long)nF);
    Serial.println(" nanoFarads");
    display.print((long)nF);
    display.println(" nF");
    delay(500);
  }
  // Display results OLED
  display.display();

  //discharge
  digitalWrite(chargePin, LOW);
  pinMode(dischargePin, OUTPUT);
  digitalWrite(dischargePin, LOW);
  while (analogRead(analogPin) > 0) {
  }
  pinMode(dischargePin, INPUT);

}

//Function to carry out Test2
void Test_two()
{
  display.clearDisplay();
  display.setCursor(0, 0);

  pinMode(IN_PIN, INPUT);
  digitalWrite(OUT_PIN, HIGH);
  int val = analogRead(IN_PIN);
  digitalWrite(OUT_PIN, LOW);

  if (val < 1000)
  {
    pinMode(IN_PIN, OUTPUT);
    // Cu = VA2 * C1 / (VA3 - VA2)
    float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
    Serial.print(F("Capacitance Value = "));
    Serial.print(capacitance, 3);
    Serial.print(F(" pF ("));
    Serial.print(val);
    Serial.println(F(") "));
    display.print(capacitance, 3);
    display.print(" pF");
  }
  else
  {
    pinMode(IN_PIN, OUTPUT);
    delay(1);
    pinMode(OUT_PIN, INPUT_PULLUP);
    unsigned long u1 = micros();
    unsigned long t;
    int digVal;
    do
    {
      digVal = digitalRead(OUT_PIN);
      unsigned long u2 = micros();
      // condition ? result_if_true : result_if_false
      t = u2 > u1 ? u2 - u1 : u1 - u2;
    } while ((digVal < 1) && (t < 400000L));

    pinMode(OUT_PIN, INPUT);
    val = analogRead(OUT_PIN);

    //discharge
    digitalWrite(IN_PIN, HIGH);
    int dischargeTime = (int)(t / 1000L) * 5;
    delay(dischargeTime);
    pinMode(OUT_PIN, OUTPUT);
    digitalWrite(OUT_PIN, LOW);
    digitalWrite(IN_PIN, LOW);

    float capacitance = -(float)t / R_PULLUP
                        / log(1.0 - (float)val / (float)MAX_ADC_VALUE);
    Serial.print(F("Capacitance Value = "));
    if (capacitance > 1000.0)
    {
      Serial.print(capacitance / 1000.0, 2);
      Serial.print(F(" uF"));
      display.print(capacitance / 1000.0, 2);
      display.print(" uF");
    }
    else
    {
      Serial.print(capacitance, 2);
      Serial.print(F(" nF"));
      display.print(capacitance, 2);
      display.print(" nF");
    }

    Serial.print(F(" ("));
    // condition ? result_if_true : result_if_false
    Serial.print(digVal == 1 ? F("Normal") : F("HighVal"));
    Serial.print(F(", t= "));
    Serial.print(t);
    Serial.print(F(" us, ADC= "));
    Serial.print(val);
    Serial.println(F(")"));
    display.setCursor(0, 15);
    display.print(t);
    display.print(" ");
    display.print(val);
  }
  while (millis() % 1000 != 0);
  display.display();
}

//Function to carry out Test3
void Test_three(void)
  
  {
  boolean exitloop = false;
  while(exitloop != true)
  {
    if (!active)
      {
      active = true;
      triggered = false;
      digitalWrite (pulsePin, HIGH); 
      startTime = micros ();  
      }
  
    if (active && triggered)
      {
      active = false;
      Serial.print ("Capacitance = ");
      Serial.print (duration * 1000 / resistance);
      Serial.println (" nF");
      triggered = false;
      //OLED
      display.print((duration * 1000 / resistance));
      display.println(" nF");
      display.display();
      delay (3000);
      exitloop = true; //exit when test finished. 
      }
  }
}

// Function to handle button press start display to OLED and serial Mon
void TestButton(int which_button)
{
  test_count ++;
  Serial.print("testcount: ");
  Serial.println(test_count);
  Serial.print("Buttonpressed: ");
  Serial.println(which_button);
  Serial.println ("Testing");
  display.clearDisplay();
  display.setCursor(7, 10);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("TESTING...");
  display.display();
}

//Function to Display elasped time to OLED and Serial Monitor called from test1 function
void Display_time(unsigned long elaspedTime)
{
  Serial.print(elaspedTime);
  Serial.print(" mS    ");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(elaspedTime);
  display.println(" mS");
}

// Function to display init screen in setup
void Display_init()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("Cap Meter");
  display.setCursor(0, 15);
  display.print("G. Lyons");
  display.display();
  delay(1500); 
  display.clearDisplay();
  
  display.setTextSize(1);
  
  display.setCursor(0, 0);
  display.print("Test 1 right");
  display.setCursor(0, 15);
  display.print("Range 1uF - 4F");
  display.display();
  delay(1500);
  display.clearDisplay();
  
  display.setCursor(0, 0);
  display.print("Test 2 center");
  display.setCursor(0, 15);
  display.print("Range 18pF - 470uF");
  display.display();
  delay(1500);
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print("Test 3 Left");
  display.setCursor(0, 15);
  display.print("Range 4.7nF - 180uF");
  display.display();
  delay(1500);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Ready");
  display.display();
  
}
//******************* EOF *****************

