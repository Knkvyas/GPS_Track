// original code gps to thingspeak

#include <SoftwareSerial.h>       //Software Serial library
SoftwareSerial espSerial(2, 3);   //Pin 2 and 3 act as RX and TX. Connect them to TX and RX of ESP8266      
#include <ThingSpeak.h>
SoftwareSerial Serial1(2,3);     //make RX arduino line is pin 2, make TX arduino line is pin 3.
SoftwareSerial gps(10,11);  // tx = 10 and Rx = 11

#define DEBUG true
String mySSID = "*****";       // WiFi SSID
String myPWD = "*********"; // WiFi Password
String myAPI = "************";   // API Key of channel
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD = "field1"; // Same Channel field 1
String myFIELD2 = "field2"; // Same Channel field 2
double sendVal; //latitude
double sendVal2; // longitude

int WriteChannelID;

int i=0,k=0;
int  gps_status=0;
String latitude=""; 
String logitude="";                       

String gpsString="";
char *test="$GPGGA";

void setup() 
{
  Serial.begin(9600);
  delay(2000);
  
  espSerial.begin(115200);
  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   //Connect to WiFi network
  
  gps.begin(9600);
  get_gps();
  show_coordinate();
  gps.end();
  Serial.println("System Ready..");
}

void loop() 
{
  k=0;
  Serial.println("System Ready..");
  while(k<1000)
  {
    
    Serial.println("Latitude:"+String(sendVal));
    Serial.println("Longitude:"+String(sendVal2));
    k++;
  gps.begin(9600);
  get_gps();
  gps.end();
  Serial.println("Start Printing");
  show_coordinate();
  
   //data to cloud
    sendVal = latitude.toDouble(); // Send latitude
    sendVal2 = logitude.toDouble(); // send longitude
    
    String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD +"="+String(sendVal)+"&"+myFIELD2+"="+String(sendVal2);

    //String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD2 +"="+String(sendVal2);
    
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">"); 
    espSerial.println(sendData);
    Serial.print("Value to be sent field 1: ");
    Serial.println(sendVal);
    Serial.print("Value to be sent field 2: ");
    Serial.println(sendVal2);
     
    espData("AT+CIPCLOSE=0",1000,DEBUG);
    delay(10000);
  
  // end
  
  Serial.println("Done Printing");
  delay(5000);
  k=990;
  delay(1);
 }
}
// esp wala code

String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    //Serial.print(response);
  }
  return response;
}

// end




void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (gps.available()>0)                //Serial incoming data from GPS
   {
    char inChar = (char)gps.read();
     gpsString+= inChar;                    //store incoming data from GPS to temporary string str[]
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])       //check for right string
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    logitude="";
    coordinate2dec();
        i=0;x=0;
    str_lenth=0;
   }
}

void show_coordinate()
{
    Serial.print("Latitude:");
    Serial.println(latitude);
    Serial.print("Longitude:");
    Serial.println(logitude);
}

void coordinate2dec()
{
    
    //j=0;
    String lat_degree="";
    for(i=18;i<20;i++)          //extract latitude from string
      lat_degree+=gpsString[i];
    
    String lat_minut="";
    for(i=20;i<28;i++)
      lat_minut+=gpsString[i];
    
    String long_degree="";
    for(i=30;i<33;i++)          //extract longitude from string
      long_degree+=gpsString[i];
      
    String long_minut="";
    for(i=33;i<41;i++)
      long_minut+=gpsString[i];
      
     float minut= lat_minut.toFloat();
     minut=minut/60;
     float degree=lat_degree.toFloat();
     latitude=degree+minut;
     
     minut= long_minut.toFloat();
     minut=minut/60;
     degree=long_degree.toFloat();
     logitude=degree+minut;
}
