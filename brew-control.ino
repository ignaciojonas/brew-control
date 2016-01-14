#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

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
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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
unsigned long currentmillis;

void setup(void)
{
  lcd.begin(16, 2);
  lcd.print("Brew Contol");
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
  currentmillis = millis();
  readkeys(); // Read keypad.
  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempC(tempDeviceAddress);
  lcd.setCursor(0, 2);
  String strOut = "CT:" + String(tempC) + " ST:" + String(setTemp); 
  lcd.print(strOut);
  if(currentmillis-switchTime > 120000) { //Wait 2 Minutes to turn on/off the fridge.
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
            cleanLine(0);
            lcd.print("Uptime");
            cleanLine(2);
            lcd.print(uptime());
            delay(2000);
            cleanLine(0);
            lcd.print("Brew Control");
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

char *uptime() // Function made to millis() be an optional parameter
{
  return (char *)uptime(millis()); // call original uptime function with unsigned long millis() value
}

char *uptime(unsigned long milli)
{
  static char _return[32];
  unsigned long secs=milli/1000, mins=secs/60;
  unsigned int hours=mins/60, days=hours/24;
  milli-=secs*1000;
  secs-=mins*60;
  mins-=hours*60;
  hours-=days*24;
  sprintf(_return,"%d days %2.2d:%2.2d:%2.2d", (byte)days, (byte)hours, (byte)mins, (byte)secs);
  return _return;
}

void cleanLine(int line){
   lcd.setCursor(0, line);
   lcd.print("                ");
   lcd.setCursor(0, line);
}


