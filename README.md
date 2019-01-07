# Project 1: Get data from sensors and send it to server

## 	Product specification

Get temperature, GSM coorordinates from sensors and send it to the server by module SIM800

##	Engineering Specification
* Kit Arduino Uno R3   
* DS18B20 sensor  
*	Adapter 5V-3A   
* LCD 16x2  
* Module SIM800A mini ver 1   
##	Hardware Specification  
DS18B20 get temperature from ambience and send to arduino, note that it's digital data (another temperature sensors send analog data to arduino)  
I use service GSM of SIM800A to get the coordinates of the nearest BTS, and use GPRS to send data to server.
## 	Software Specification  
* arduino: use Aruino IDE, C/C++ language
  * communicate between arduino and module SIM by serial port
  * use AT commands to control SIM800A
* web : C#, CSS, ASP.NET    
## Function
* Get temperature
```C++
#include <DallasTemperature.h>

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

String GetTemperature() {
  lcd.setCursor(0, 1);
  lcd.print("Get Temperature ");

  static String T;
  sensors.requestTemperatures();
  T = String(sensors.getTempCByIndex(0) - 1);
  delay(1000);
  return T;
}

```  
* Get coordinate
```C++
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

  return loc;
}

```  
* Send data to server:we create a server online and have it's Ip address, we also design a web (backend: ASP.NET, SQL, front-end: CSS), this is a function in arduino to send data to server
```C++
void Send2Server(String server, String location, String temperature) {
 lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("send 52.26.96.37");

  Serial.println("Sending...");
  sendCmd("AT+HTTPINIT\r\n", 500, 1);
  sendCmd("AT+HTTPPARA=\"CID\",1\r\n", 500, 1);
  sendCmd("AT+HTTPPARA=\"URL\"," + server + "?temperature=" + temperature + "&coor=" + location + "\"""\r\n", 500, 0);
  sendCmd("AT+HTTPACTION=0\r\n", 1000, 1);
  sendCmd("AT+HTTPTERM\r\n", 500, 0);
  sendCmd("AT+SAPBR=0,1\r\n", 500, 0);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Upload Complete!");

}

```  
* Resend data when reveived message from server:   
```C++
void Check_message()
{

  String message = sendCmd("AT+CMGL=\"ALL\"\r\n", 1000, 1); //list all message in storage
  Serial.println("mes: " + message);
  if (message.indexOf("Nexmo") != -1) {
  ResetBoard(500);

  }

```  

