#include<SoftwareSerial.h>
SoftwareSerial Serial1(2,3);     //make RX arduino line is pin 2, make TX arduino line is pin 3.
SoftwareSerial gps(10,11);

boolean No_IP=false;
String IP="";

String webpage="";
int i=0,k=0;
int  gps_status=0;
String latitude=""; 
String logitude="";                       

String gpsString="";
char *test="$GPGGA";

void setup() 
{
  Serial1.begin(9600);
  Serial.begin(9600);
      delay(2000);
      gps.begin(9600);
      get_gps();
      show_coordinate();
      gps.end();
      Serial1.begin(9600);
      Serial.println("System Ready..");
}

void loop() 
{
  k=0;
  while(k<1000)
  {
    k++;

   while(Serial1.available())
   {
    if(Serial1.find("0,CONNECT"))
    {
      Serial1.end();
  gps.begin(9600);
  get_gps();
  gps.end();
  Serial1.begin(9600);
  Serial1.flush();
      Serial.println("Start Printing");
      show_coordinate();
      Serial.println("Done Printing");
      delay(5000);
      delay(1000);
      k=1200;
      break;
    }
  }
  delay(1);
 }
}

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
