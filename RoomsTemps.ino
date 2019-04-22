#include <Ethernet.h>
#include <DHT.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <ArduinoJson.h>
byte portFloor       = 2;
byte portRoom        = 3;
byte portExternal    = 5;
byte portSwitchOne   = 7;
byte portSwitchTwo   = 8;
byte portSwitchThree = 9;
byte portSwitchFour  = 10;
byte ActivityLedPort = 13;
byte SoundPort       =  6;

DHT dhtRoom(portRoom, DHT22);
DHT dhtFloor(portFloor, DHT22);
DHT dhtExternal(portExternal, DHT22);
EthernetServer server(80);  //server port

void setup() 
{
  Serial.begin(9600);
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
  byte ip[] = { 192, 168, 1, 100 };                      //ip in lan (that's what you need to use in your browser. ("192.168.1.178")
  byte gateway[] = { 192, 168, 1, 1 };                   //internet access via router
  byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask 
  Ethernet.begin(mac, ip, gateway, subnet);
                        
  server.begin();  
  dhtRoom.begin(); 
  dhtFloor.begin(); 
  dhtExternal.begin();
     
  pinMode(6,INPUT);
  pinMode(portSwitchOne, OUTPUT);
  pinMode(portSwitchTwo, OUTPUT);
  pinMode(portSwitchThree, OUTPUT);
  pinMode(portSwitchFour, OUTPUT);  
  pinMode(ActivityLedPort, OUTPUT);
  
  digitalWrite(portSwitchOne,   LOW); 
  digitalWrite(portSwitchTwo,   LOW); 
  digitalWrite(portSwitchThree, LOW); 
  digitalWrite(portSwitchFour,  LOW);   
  digitalWrite(ActivityLedPort, HIGH);  
}
void loop() 
{       
  String readString;  
  EthernetClient client = server.available();
  if (client) 
  {
    while (client.connected()) 
    {   
      if (client.available()) 
      {
        char c = client.read();             
        if (readString.length() < 100) 
        {
          readString += c;
        }         
         if (c == '\n') 
         {          
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");   
           client.println();
           client.println(F("<HTML><HEAD><meta name='apple-mobile-web-app-capable' content='yes' /><meta name='apple-mobile-web-app-status-bar-style' content='black-translucent'/>"));
           client.println(F("<TITLE>Home Temp</TITLE>"));
           client.println(F("<style>#customers{font-family:\"Trebuchet MS\",Arial, Helvetica,sans-serif;border-collapse:collapse;width:100%;} #customers td, #customers th{border:1px solid #ddd;padding:8px;text-align:Center;} #customers tr:nth-child(even){background-color:#f2f2f2;} #customers tr:hover{background-color:#ddd;} #customers th{padding-top:12px;padding-bottom:12px;text-align:Center;background-color:#4CAF50;color:black;}a:link, a:visited {background-color:white;border: 2px solid #4CAF50;color:Black;padding:14px 25px;text-align:center;text-decoration:none;display:inline-block;} a:hover, a:active {background-color:#4CAF50;}</style>"));
           client.println(F("</HEAD>"));
           client.println(F("<BODY>"));                    
           client.println(F("<Center><a href=\"/?button1\"\">Restart network hardware</a></Center><br/><br/>"));
           client.println(F("<Center><a href=\"/?button2\"\">Temperatures</a></Center><br/><br/>"));               
           client.println(F("</BODY>"));
           client.println(F("</HTML>"));
           delay(1);
                     
           if (readString.indexOf("?button1") > 0)
           {
               RestartAllSwitches();
           }           
           if (readString.indexOf("?button2") > 0)
           {
             GetTempsButtonClicked(client);       
           }   
           if(readString.indexOf("?GetTempsJSON") > 0)
           {
              printTempData(client);
           }
           client.stop();                  
           readString="";             
         }
       }
    }
  }
}
void GetTempsButtonClicked(EthernetClient client)
{  
    float Temps[2];   
    client.println("<br />");
    String line = "<table id=\"customers\"><tr><th>Location</th><th>Temperature</th><th>Humidity</th><th>Sound Level</th></tr>";              
    client.print(line);      
    GetExternalTempData(Temps);             
    line = "<tr><td>External</td><td>" +String(Temps[0]) +  (char)176 +  "C</td><td>"+ String(Temps[1]) +  "%</td><td>NA</td></tr>";
    client.print(line);                      
    GetFloorTempData(Temps); 
    line = "<tr><td>Floor</td><td>"+String(Temps[0]) +  (char)176 +  "C</td><td>"+ String(Temps[1]) +  "%</td><td>NA</td></tr>";
    client.print(line);                      
    GetRoomTempData(Temps); 
    line = "<tr><td>Room</td><td>"+String(Temps[0]) +  (char)176 +  "C</td><td>"+ String(Temps[1]) +  "%</td>"+ (digitalRead(6) == 1 ? "<td><b>HIGH</b>" : "<td> LOW") + "</td></tr></table>";           
    client.print(line);   
}
void GetRoomTempData(float (& TempData)[2])
{  
  TempData[0] =  dhtRoom.readTemperature();
  TempData[1] =  dhtRoom.readHumidity(); 
}
void GetFloorTempData(float (& TempData)[2])
{  
  TempData[0] =  dhtFloor.readTemperature();
  TempData[1] =  dhtFloor.readHumidity(); 
}
void GetExternalTempData(float (& TempData)[2])
{  
  TempData[0] =  dhtExternal.readTemperature();
  TempData[1] =  dhtExternal.readHumidity();
}
void RestartAllSwitches()
{  
    digitalWrite(portSwitchOne, HIGH);
    digitalWrite(portSwitchTwo, HIGH);
    digitalWrite(portSwitchThree, HIGH);
    digitalWrite(portSwitchFour, HIGH); 

    int counter = 1;                              
    for (int i=1; i <= 15; i++)
    {                                                               
      digitalWrite(ActivityLedPort, HIGH);  
      delay(1000);                                
      digitalWrite(ActivityLedPort, LOW);  
      delay(1000);                                  
    }  
                               
    digitalWrite(portSwitchOne, LOW);
    digitalWrite(portSwitchTwo, LOW);
    digitalWrite(portSwitchThree, LOW);
    digitalWrite(portSwitchFour, LOW);

}
void webClientPrintResponse(EthernetClient webClient, JsonObject& roots)
{                            
                             char buffer[250];                             
                             roots.printTo(buffer, sizeof(buffer));                             
                             webClient.println(F("HTTP/1.1 200 OK"));
                             webClient.println(F("Content-Type: text/html"));
                             webClient.println(F("Connection: close"));
                             webClient.println();
                             webClient.println(buffer); 
} 
void printTempData(EthernetClient clientTemp)
{     
                             float ExtTemps[2];  
                             float RoomTemps[2];  
                             float FloorTemps[2];  
                             GetExternalTempData(ExtTemps);
                             GetFloorTempData(FloorTemps); 
                             GetRoomTempData(RoomTemps); 
                                                          
                             StaticJsonBuffer<250> jsonBuffer;                             
                             JsonObject& root            = jsonBuffer.createObject();                             
                             
                             JsonObject& ExternalTems    = root.createNestedObject("External");                                
                             
                             ExternalTems["Temperature"] = ExtTemps[0];
                             ExternalTems["Humidity"]    = ExtTemps[1];
                             ExternalTems["SoundLevel"]  = "NA";                                                           
                             
                             JsonObject& FloorTems    = root.createNestedObject("Floor");
                             FloorTems["Temperature"] = FloorTemps[0];
                             FloorTems["Humidity"]    = FloorTemps[1];
                             FloorTems["SoundLevel"]  = "NA";  
                            
                             JsonObject& RoomTems    = root.createNestedObject("Room");
                             RoomTems["Temperature"] = RoomTemps[0];
                             RoomTems["Humidity"]    = RoomTemps[1];
                             RoomTems["SoundLevel"]  = (digitalRead(6) == 1 ? "HIGH" : "LOW");
                             //root.prettyPrintTo(Serial);                                                                               
                             webClientPrintResponse(clientTemp,root);                                
}
