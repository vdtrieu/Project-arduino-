#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/wdt.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial serialSIM800(11, 12);
LiquidCrystal lcd(10, 9, 7, 6, 5, 4);
static String server = "\"http://52.26.96.37/WebForm1.aspx";



void setup() {
  lcd.begin(16, 2);
  lcd.print("Seting up...");
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while (!Serial);// wait for serial port to connect. Needed for native USB

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  Serial.println("Start!");
  sendCmd("AT\r\n", 500, 1);
  sendCmd("AT+CGATT=1\r\n", 500, 1);
  sendCmd("AT+CMGF=1\r\n", 500, 1);
  sendCmd("AT+CMGD=1,4\r\n", 500, 1);
  sendCmd("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n", 500, 1);
  sendCmd("AT+SAPBR=3,1,\"APN\",\"internet\"\r\n", 500, 1);


  lcd.setCursor(0, 0);
  Serial.println("Setup Complete!");
  lcd.print("Setup Complete!");



}

void loop() {
  // Start//
  String location = "";
  String t = "";
  location = getLocation();
  t = GetTemperature();
  Serial.print("Temperature: " + t + "\n");
  Serial.print("Location: " + location + "\n");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: " + t + " oC");
  //  Serial.println(location);
  if (location.length() != 0 && t.toInt() < 50 ) {
    Send2Server(server, location, t);
  } else {
    Serial.println("Fail !");
    //    delay(2000);
    //    ResetBoard(WDTO_1S);
    lcd.setCursor(0, 1);
    lcd.print("Fail !          ");
  }

  Delay(1200000);
  ResetBoard(WDTO_1S);
}
String sendCmd(String command,
               const int delay, boolean debug) {
  String strCmd = "";

  serialSIM800.print(command); // send the read character to the sim800a

  long int time = millis();

  while ((time + delay) > millis()) {
    while (serialSIM800.available()) {

      // The MS has data so display its output to the serial window
      char c = serialSIM800.read(); // read the next character.
      strCmd += c;
    }
  }

  if (debug) {
    Serial.print(strCmd);
  }
  return strCmd;
}
String getLocation() {
  String loc = "";
  String rawdata = "";

  lcd.setCursor(0, 1);
  lcd.print("Getting Location");

  sendCmd("AT+SAPBR=1,1\r\n", 1000, 1);
  sendCmd("AT+SAPBR=2,1\r\n", 500, 1);
  rawdata = sendCmd("AT+CIPGSMLOC=1,1\r\n", 6000, 1);

  //Get longtitue and latitude
  if (rawdata.indexOf("+CIPGSMLOC: 0,") != -1) {
    loc = rawdata.substring(33, 53);


  }
  //  sendCmd("AT+SAPBR=0,1\r\n", 1000, 1);

  return loc;
}
void Send2Server(String server, String location, String temperature) {
  //  String a = "AT+HTTPPARA=\"URL\"," + server + "&coor=" + location + "\"""\r\n";
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("send 52.26.96.37");

  Serial.println("Sending...");
  sendCmd("AT+HTTPINIT\r\n", 500, 1);
  sendCmd("AT+HTTPPARA=\"CID\",1\r\n", 500, 1);
  sendCmd("AT+HTTPPARA=\"URL\"," + server + "?temperature=" + temperature + "&coor=" + location + "\"""\r\n", 500, 0);
  //  Serial.println("..." + a);
  sendCmd("AT+HTTPACTION=0\r\n", 1000, 1);
  sendCmd("AT+HTTPTERM\r\n", 500, 0);
  sendCmd("AT+SAPBR=0,1\r\n", 500, 0);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Upload Complete!");

}
String GetTemperature() {
  lcd.setCursor(0, 1);
  lcd.print("Get Temperature ");

  static String T;
  sensors.requestTemperatures();
  T = String(sensors.getTempCByIndex(0) - 1);
  delay(1000);
  return T;
}
void ResetBoard( uint8_t time)
{
  wdt_enable(time);
  while (1) {}
}
void Delay(float delayTime) {
  unsigned long endTime = millis() + delayTime;
  while (millis() < endTime)
  {
    //  Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
    if (serialSIM800.available()) {
      //      Serial.write(serialSIM800.read());
      String SIM_output = serialSIM800.readString();
      Serial.println(SIM_output);
      if (SIM_output.indexOf("CMTI") != -1) {
        lcd.setCursor(0, 1);
        lcd.print("New message!      ");
        Check_message();
      }

    }
    //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
    if (Serial.available()) {
      serialSIM800.write(Serial.read());
    }
  }
}
void Check_message()
{

  String message = sendCmd("AT+CMGL=\"ALL\"\r\n", 1000, 1); //list all message in storage
  if (message.indexOf("Nexmo") != -1) {
    Blink_Led();
    ResetBoard(WDTO_1S);

  }
}
void Blink_Led(){
//  int t = millis()+ 2000;
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(2000);                       // wait for a second
      digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
      delay(2000); 
  }

