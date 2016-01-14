#include <OneWire.h>
#include <DallasTemperature.h>
#include <LCD4Bit_mod.h> 

//Temperature Sensor
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

//From 12 to 9.
//12 Max Precision
//9 Min precision
#define TEMPERATURE_PRECISION 9 

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// We'll use this variable to store the device address
DeviceAddress tempDeviceAddress; 

//Relay
int COOLER = 12;

//LCD
//number of lines in display=2
LCD4Bit_mod lcd = LCD4Bit_mod(2); 

//Possible Keys
//30 Right Key - Key 0
//150 Up Key - Key 1
//360 Down Key - Key 2
//535 Left Key - Key 3
//760 Select Key - Key 4
int  key_val[5] ={30, 150, 360, 535, 760};
int NUM_KEYS = 5;
char tempMessage[16];
int key=-1;
  int oldkey=-1;
// Set Temperature.
int setTemp;

unsigned long switchTime;
unsigned long time;

void setup(void)
{
  lcd.init();
  lcd.clear();
  lcd.printIn("Brew Contol");
  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
	// set the resolution to TEMPERATURE_PRECISION bit.
	sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
  setTemp = 20;
  switchTime = 0;
  pinMode(COOLER, OUTPUT);  
}

void loop(void)
{ 
  time = millis();
  readkeys(); // Read keypad.
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempC(tempDeviceAddress);
  lcd.cursorTo(2, 0);  //line=2, x=0
  String strOut = String("CT:") + String(tempC) + String(" ST:") + String(setTemp); 
  strOut.toCharArray(tempMessage, 16);
  lcd.printIn(tempMessage);
  if(time-switchTime > 120000) { //Wait 2 Minutes to turn on/off the fridge.
    if (tempC > setTemp){
      digitalWrite(COOLER, HIGH);  //Turn on the fridge
    }
    else
    {
      digitalWrite(COOLER, LOW);  
    }
    switchTime = millis();
  }
   
}

//Reads the keypad
void readkeys(void){
  int key_in = analogRead(0);       // read the value from the sensor  
  key = get_key(key_in);        // convert into key press
  if (key != oldkey)            // if keypress is detected
  {
    delay(50);                  // wait for debounce time
    key_in = analogRead(0);     // read the value from the sensor  
    key = get_key(key_in);      // convert into key press
    if (key != oldkey)        
    {     
      oldkey = key;
      switch (key) {
          case 1:
            setTemp++;           // increase set temperature
            break;
          case 2:
            setTemp--;          // decrese set temperature
            break;
          case 4:
            lcd.cursorTo(1, 0);
            String strOut = String("Running Time:") + String(time); 
            strOut.toCharArray(tempMessage, 16);
            lcd.printIn(tempMessage);
            break;
        }
    }
  }
}

//Return the pressed key
int get_key(unsigned int input)
{
  int k;
  for (k = 0; k < NUM_KEYS; k++)
  {
    if (input < key_val[k])
      return k;
  }
  return -1;
}
