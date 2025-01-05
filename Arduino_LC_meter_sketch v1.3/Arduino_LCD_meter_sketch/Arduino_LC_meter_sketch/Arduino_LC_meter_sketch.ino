/* Arduino_LC_meter_sketch.ino

youtube channel : https://www.youtube.com/@TeknoTrek

 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   // set the LCD address to 0x27,
                                    // with 2 x 16 char lines   
#include <EEPROM.h>               // include the EEPROM library
#include <FreqCount.h>            // and the FreqCount library

// ====================================================================
// declaration of global variables
const float Pi = 3.14159;     // define Pi
const float FourPiSqrd = 39.4784;  // and Pi squared
const float uHmult = 1.0e6;   // multiplier for converting H into uH
const float mHmult = 1.0e3;   // multiplier for converting H into mH
const float pFmult = 1.0e12;  // multiplier for converting F into pF
const float nFmult = 1.0e9;   // multiplier for converting F into nF
const float uFmult = 1.0e6;   // multiplier for converting F into uF
const float C2val = 1.000e-9;   // nominal value of C2 (1nF = 1 x 10^-9F)
                              // (change to actual value if known)
const int CLbarPin = 2;   // digital IO pin 2 for C/L-bar sensing
const int RelayPin = 3;   // digital IO pin 3 for driving RLY1
const int DecrPin = 4;    // digital IO pin 4 for sensing DECR pos of S3
const int OscSigPin = 5;  // digital IO pin 5 for input of oscillator signal
const int IncrPin = 6;    // digital IO pin 6 for sensing INCR pos of S3
const int CalLkPin = 7;   // digital IO pin 7 for sensing CAL link LK1

float C1val;     // C1's calculated value after calibration (in F)
float CXval;     // calculated value for Cx (in F)
float L1val;     // calculated value for L1 after calibration (H)
float LXval;     // calculated value for Lx (in H)
float F1sqrd;    // calc value for Freq1 squared (as a float)
float F2sqrd;    // calc value for Freq2 squared (as a float)
float F3sqrd;    // calc value for Freq3 squared (as a float)
float CF = 1.0;  // calibration factor, set by nudging via S3

long Fcount;   // raw frequency count figure (from GetFrequency function)
long Freq1;    // measured Freq1 in Hz (L1 & C1 only)
long Freq2;    // measured Freq2 in Hz (L1 & (C1 + C2))
long Freq3;    // measured Freq3 in Hz (C1+Cx/L1 or C1/L1+Lx)

byte MTorNot = 0;   // flag: 0 = EEPROM addresses empty (== 255)

// ====================================================================
// setup function begins here
// ====================================================================
void setup()
{
  pinMode(CLbarPin, INPUT_PULLUP);    // make pin 2 an input with pullup
  pinMode(RelayPin, OUTPUT);          // but make pin3 an output
  digitalWrite(RelayPin, LOW);        // and initialise it to LOW
  pinMode(DecrPin, INPUT_PULLUP);     // make pin 4 an input with pullup
  pinMode(OscSigPin, INPUT);          // make pin 5 an input with no pullup
  pinMode(IncrPin, INPUT_PULLUP);     // make pin 6 an input with pullup
  pinMode(CalLkPin, INPUT_PULLUP);    // and make pin 7 an input with pullup

  lcd.init();             // initialise LCD
  lcd.backlight();
  lcd.setCursor(3,0);     // and show opening screen
  lcd.print("TEKNOTREK");
  lcd.setCursor(3,1);
  lcd.print("LC Meter");
  delay(2000);            // then pause for 2 seconds
  lcd.clear();      // then clear screen again (sets cursor to 0,0)
  
  int i = 0;          // just a counter for the loop below
  // now check to see if CF is saved in EEPROM (addresses 0 - 3)
  for (i = 0; i < 4; i++)
  {
    if (EEPROM.read(i) != 255)
    {
      MTorNot = 1;  // address not empty, so raise flag
    }
  } // end of loop checking EEPROM addresses 0-3

  if (MTorNot == 0)      // if it is empty (i.e., fresh start)
  {
    EEPROM.put(0, CF);  // save CF in EEPROM 
  }
  else
  {
    EEPROM.get(0, CF);  // otherwise retrieve saved CF from EEPROM
  }
 // now do a calibration
  {
    if(digitalRead(CLbarPin) == HIGH)
    {
      lcd.print("S1 set for C: OK"); // S1 in 'C' position, so confirm
    }
    else
    {
      lcd.print("Fit shorting bar"); // S1 in 'L' position, so signal
    }                                // need for shorting bar
    
    lcd.setCursor(0,1);               // now show "Now calibrating"
    lcd.print("Now calibrating");     // on second line (both cases)
    FindC1andL1();    // now do calibration, to find F1, F2, C1 & L1
    DispCalData();    // then display C1 & L1 (pausing for 3 secs)
    lcd.clear();
    lcd.print("Calibration done");
    lcd.setCursor(0,1);
    lcd.print("Ready to measure");
    delay(1000);
  }
} // end of setup function

 // ===================================================================
 // main loop() function begins here
 // ===================================================================
void loop()
{
  lcd.clear();                // first clear screen, then
  GetFrequency();             // go get current osc frequency
  Freq3 = Fcount;             // and copy into Freq3
  float F3 = float(Freq3);    // then convert to a float
  F3sqrd = pow(F3, 2.0);      // so it can be squared
  String Cdisp;               // declare C display string
  String Ldisp;               // declare L display string
  
  if(digitalRead(CLbarPin) == HIGH)  // if we're measuring a C
  {
    CXval = C1val * CF * (float(F1sqrd/F3sqrd) - 1.0); // work it out
    
    if(CXval < 1.0e-9)     // if CXval < 1nF
    {
      float CXpF = CXval * pFmult;  // get equiv value in pF
      Cdisp = " Cx = ";  // then assemble top line string
      Cdisp += String(CXpF, 3);
      Cdisp += " pF";
    }
    else    // value must be 1nF or more
    {
      if(CXval < 1.0e-6)    // but check if it's less than 1uF
      {
        float CXnF = CXval * nFmult;  // if so, get equiv value in nF
        Cdisp = "Cx = ";   // then assemble top line string
        Cdisp += String(CXnF, 3);
        Cdisp += " nF";
      }
      else    // value must be 1uF or more
      {
        float CXuF = CXval * uFmult;  // so get equiv value in uF
        Cdisp = "Cx = ";  // then assemble top line string
        Cdisp += String(CXuF, 3);
        Cdisp += " uF";  
      }
    }
    lcd.print(Cdisp);  // now display the cap value string
  }
  else    // CLbarPin == LOW, so we must be measuring an L
  {
    LXval = L1val * CF * (float(F1sqrd/F3sqrd) - 1.0); // work it out
    
    if(LXval < 1.0e-3)     // if LXval < 1mH
    {
      float LXuH = LXval * uHmult; // convert to uH 
      Ldisp = " Lx = "; // then assemble top line string
      Ldisp += String(LXuH, 3);
      Ldisp += " uH";
    }
    else      // value must be 1mH or more
    {
      if(LXval < 1.5e-1)
      {
        float LXmH = LXval * mHmult; // convert to mH 
        Ldisp = " Lx = "; // then assemble top line string
        Ldisp += String(LXmH, 3);
        Ldisp += " mH";
      }
      else
      {
        Ldisp = "  Over Range!";     
      }
    }
    lcd.print(Ldisp);  // show inductance value or over range msg
  }
  lcd.setCursor(1,1);     // then show F3 on second line
  String F3disp = "F3 = ";
  F3disp += String(Freq3);
  F3disp += " Hz";
  lcd.print(F3disp);
  
  // next section is for calibration nudging, only when LK1 is fitted 
  if(digitalRead(CalLkPin) == LOW)  // if LK1 is fitted, check S3
  {               // because it looks like some nudging is needed
    if(digitalRead(IncrPin) == LOW) // if it's increment,
    {
      CF = CF * 1.005;   // nudge up CF by 0.5%
      EEPROM.update(0, CF); // then resave new CF in EEPROM 
    }
    if(digitalRead(DecrPin) == LOW) // or if it's decrement,
    {
      CF = CF * 0.995;  // nudge CF down by 0.5%
      EEPROM.update(0, CF); // then resave new CF in EEPROM 
    }
    // note that nudging only occurs when CalLkPin is LOW (i.e., LK1
    // fitted) AND either IncrPin or DecrPin is taken LOW by S3.
    // Otherwise nothing occurs!
  }
  delay(1500);
}      // end of main loop

// =====================================================================
// FindC1andL1() fn: for initial calibration
// =====================================================================
void FindC1andL1()
{
  digitalWrite(RelayPin, LOW);  // make sure RLY1 is off (for C1 only)
  GetFrequency();               // go get Fosc
  Freq1 = Fcount;               // and copy into Freq1
  digitalWrite(RelayPin, HIGH); // now turn on relay to add C2
  delay(1000);                  // pause to allow osc to settle
  GetFrequency();               // then go get Fosc again
  Freq2 = Fcount;               // and copy into Freq2
  digitalWrite(RelayPin, LOW);  // then turn off relay again
  
  // now find the values of C1 and L1, knowing C2, Freq1 & Freq2
  float F1 = float(Freq1);      // first convert F1 and F2 to floats
  float F2 = float(Freq2);
  F1sqrd = pow(F1, 2.0);       // then find their squares
  F2sqrd = pow(F2, 2.0);
  float F1sqlessF2sq = (F1sqrd - F2sqrd); // & their difference
  C1val = C2val * float(F2sqrd / F1sqlessF2sq);    // then work out C1
  L1val = 1.0/(FourPiSqrd * float(F1sqrd) * C1val);  // and L1
  return; 
}  // end of FindC1andL1 function

// =====================================================================
// GetFrequency() fn: to measure IC1's current oscillating frequency
// =====================================================================
void GetFrequency()
  {
  FreqCount.begin(1000);   // start counter, with a gating time of 1 sec  
  while (!FreqCount.available())  // loop until Fcount is available
    {
    }
  Fcount = FreqCount.read();   // when it's available, read into Fcount
  FreqCount.end();        // and close off counter before leaving
  return;
  } // end of GetFrequency function

// =====================================================================
// DispCalData() fn: to display calibration data (C1 and L1)
// =====================================================================
void DispCalData()
{
  float L1uH = L1val * uHmult; // find uH equivalent of L1val
  float C1pF = C1val * pFmult; // and the pF equivalent of C1val
  
  lcd.clear();      // first clear screen again (sets cursor to 0,0)
  String C1disp = "C1= ";  // then assemble C1 display string
  C1disp += String(C1pF, 2);
  C1disp += " pF";
  lcd.print(C1disp); // and display on top line
  lcd.setCursor(0,1);     // then move down to second line
  String L1disp = "L1= ";
  L1disp += String(L1uH, 2);
  L1disp += "uH";
  lcd.print(L1disp);  // and display on second line
  delay(3000);        // pause for 3 seconds to allow digesting
  return;         // before returning
} // end of DispCalData function

// =====================================================================
//    end of code

