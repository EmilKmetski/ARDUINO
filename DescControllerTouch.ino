#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>     
#include <DHT.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define DHTPIN  53
#define DHTTYPE DHT22     // DHT 22  (AM2302)

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

#define BUTTON_X 50
#define BUTTON_Y 160
#define BUTTON_W 100
#define BUTTON_H 50
#define BUTTON_SPACING_X 5
#define BUTTON_SPACING_Y 5
#define BUTTON_TEXTSIZE 3

//#define YP A3  
//#define XM A2
//#define YM 9
//#define XP 8  

#define YP A2  
#define XM A3
#define YM 8
#define XP 9  

//#define YP A1  
//#define XM A2  
//#define YM 7   
//#define XP 6   

//#define TS_MINX 150
//#define TS_MINY 120
//#define TS_MAXX 920
//#define TS_MAXY 940

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define SENSITIVITY 300
#define MINPRESSURE 25
#define MAXPRESSURE 1000

#define ACCURACY 0.50  
#define ERRORCORRECTION 0.30  

Adafruit_TFTLCD     tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen         ts = TouchScreen(XP, YP, XM, YM, SENSITIVITY);
Adafruit_GFX_Button buttons[6];

uint16_t buttoncolors[6] = {ILI9341_RED, ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE};
char*    buttonTitles[6] = {"Save", "Pos.1", "Pos.2","Pos.3", "UP","DOWN"};

const int RELAY[]       = {22,23};  //RELAY[0] and RELAY[1] to access the pins
const int trigPin       = 51;
const int echoPin       = 49;

const uint8_t MANUAL    = 1; //a constant to indicate manual mode
const uint8_t AUTOMATIC = 2; //a constant to indicate automatic mode

const int measureInterval     = 800;
const int displayTempInterval = 30000;

const float deskWidth        = 3;
const float deskMinPossition = 65.50;
const float deskMaxPossition = 110.50;
const char  deskName[]       = {"Kmet@`s Desk"};

float newPosition    = 0;
float oldPosition    = 0;
float targetPosition = 0;

unsigned long previousMillis            = 0;
unsigned long currentMillis             = 0;
unsigned long previousMillisDisplay     = 0;
unsigned long previousMillisTempDisplay = 0;

float memPosition[] = {5.5,10.5,11.5};
uint8_t MODE        = MANUAL;

bool btnSavePressed = false;
bool btnPos1Pressed = false;
bool btnPos2Pressed = false;
bool btnPos3Pressed = false;
bool btnUPPressed   = false;
bool btnDownPressed = false;

TSPoint p;                          
DHT     dht(DHTPIN, DHTTYPE);

void setup(void) 
{
  Serial.begin(9600); 
  InitilizeScreen();
  pinMode(RELAY[0], OUTPUT);
  pinMode(RELAY[1], OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  dht.begin();
}

void InitilizeScreen()
{
  Serial.println(F("TFT LCD test"));
#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());
  
  tft.reset();
  uint16_t identifier = tft.readID();

  Serial.println(identifier, HEX);
  
  if(identifier==0x0101)
      identifier=0x9341;      
  if(identifier==0xC0C0)
      identifier=0x9341; 
  if(identifier == 0x9325) 
  {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x4535) 
  {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x9328) 
  {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) 
  {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) 
  {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) 
  {
    Serial.println(F("Found HX8357D LCD driver"));
  } else 
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.4\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(3);  
  // x, y, w, h, outline, fill, text
  //Save Button    
  buttons[0].initButton(&tft,BUTTON_X + BUTTON_W*2 + BUTTON_SPACING_X*2,BUTTON_Y+BUTTON_SPACING_Y+BUTTON_H ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[0], ILI9341_GREENYELLOW, buttonTitles[0] , BUTTON_TEXTSIZE); 
  buttons[0].drawButton();    
  //Pos.1
  buttons[1].initButton(&tft,BUTTON_X                                  ,BUTTON_Y                          ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[1], ILI9341_GREENYELLOW, buttonTitles[1], BUTTON_TEXTSIZE); 
  buttons[1].drawButton();  
  //Pos.2
  buttons[2].initButton(&tft,BUTTON_X + BUTTON_W + BUTTON_SPACING_X    ,BUTTON_Y                          ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[2], ILI9341_GREENYELLOW, buttonTitles[2], BUTTON_TEXTSIZE); 
  buttons[2].drawButton();  
  //Pos.3
  buttons[3].initButton(&tft,BUTTON_X + BUTTON_W*2 + BUTTON_SPACING_X*2,BUTTON_Y                           ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[3], ILI9341_GREENYELLOW, buttonTitles[3], BUTTON_TEXTSIZE); 
  buttons[3].drawButton();  
  //Up
  buttons[4].initButton(&tft,BUTTON_X                                  ,BUTTON_Y+BUTTON_SPACING_Y+BUTTON_H ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[4], ILI9341_GREENYELLOW, buttonTitles[4]  , BUTTON_TEXTSIZE); 
  buttons[4].drawButton();  
  //Down
  buttons[5].initButton(&tft,BUTTON_X + BUTTON_W + BUTTON_SPACING_X    ,BUTTON_Y+BUTTON_SPACING_Y+BUTTON_H ,BUTTON_W,BUTTON_H,ILI9341_GREENYELLOW, buttoncolors[5], ILI9341_GREENYELLOW, buttonTitles[5] , BUTTON_TEXTSIZE); 
  buttons[5].drawButton();
  
  DrawPosition(1,false);
  DrawTemp();
  DrawDeskName(deskName);
}

void readResistiveTouch()
{
  digitalWrite(13, HIGH);
  p = ts.getPoint();
  digitalWrite(13, LOW);   
  pinMode(YP, OUTPUT);      //restore shared pins
  pinMode(XM, OUTPUT);  
}
void DrawPosition(int traveldirection, bool moving)
{ 
  if ((unsigned long)(currentMillis - previousMillisDisplay) >= measureInterval)
  {
    tft.fillRect(25, 80, 320, 30, ILI9341_BLACK); 
    tft.setCursor(25, 80);
    tft.setTextColor(ILI9341_GREENYELLOW);
    tft.setTextSize(3);
    if(moving)
    {
      tft.print(traveldirection == 1 ? "UP / ":"DOWN / ");  
    }
    else
    {
      tft.print("STOP / ");  
    }  
    tft.print(GetDistance(deskWidth)); 
    tft.print("cm");
  }
}
void DrawTemp()
{
  if ((unsigned long)(currentMillis - previousMillisTempDisplay) >= displayTempInterval)
  {
    tft.fillRect(25, 110, 320, 25, ILI9341_BLACK);
    tft.setCursor(25, 110);
    tft.setTextColor(ILI9341_GREENYELLOW);
    tft.setTextSize(3);  
    tft.print(dht.readTemperature());
    tft.print("C / ");
    tft.print(dht.readHumidity());
    tft.print("%"); 
    previousMillisTempDisplay = currentMillis;
  }
}
void DrawDeskName(char* deskName)
{
  tft.fillRect(0, 0, 320, 30, ILI9341_NAVY);
  tft.setCursor(50, 5);
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(3); 
  tft.print(deskName);  
}
void DrawInfo(char* data)
{  
    tft.fillRect(10, 35, 320, 35, ILI9341_BLACK);
    tft.setCursor(10, 35);
    tft.setTextColor(ILI9341_GREENYELLOW);
    tft.setTextSize(3); 
    tft.print(data); 
}

void loop(void) 
{  
   currentMillis = millis();       
   readResistiveTouch();    
   
   if(previousMillisTempDisplay <= 0)
   {
      previousMillisTempDisplay = currentMillis;
   }
      
   DrawTemp();
   
   if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
   {  
    //If the buttons are not pressed correctly swith the places of the TS_MINX,TS_MAXX AND THE places of tft.width()....
    // scale from 0->1023 to tft.width    
    p.x = map(p.x, TS_MINX, TS_MAXX,tft.width(),0);
    p.y = map(p.y, TS_MINY, TS_MAXY,tft.height(),0);
   }
   
   // go thru all the buttons, checking if they were pressed
   for (uint8_t b = 0; b < 6; b++) 
   {      
      if (buttons[b].contains(p.x, p.y)) 
      {       
        buttons[b].press(true);  // tell the button it is pressed   
        previousMillisDisplay = currentMillis;        
        switch(b)
        {
          //Button Save Pressed
          case 0:
          btnSavePressed = true;
          btnPos1Pressed = false;
          btnPos2Pressed = false;
          btnPos3Pressed = false;
          btnUPPressed   = false;
          btnDownPressed = false;
          break;
          //Button Pos 1 Pressed
          case 1:
          btnPos1Pressed = true;          
          btnPos2Pressed = false;
          btnPos3Pressed = false;
          btnUPPressed   = false;
          btnDownPressed = false;
          
          break;
          //Button Pos 2 Pressed
          case 2:
          btnPos2Pressed = true;
          btnPos1Pressed = false;          
          btnPos3Pressed = false;
          btnUPPressed   = false;
          btnDownPressed = false;
          break;
          //Button Pos 3 Pressed
          case 3:
          btnPos3Pressed = true;
          btnPos1Pressed = false;          
          btnPos2Pressed = false;
          btnUPPressed   = false;
          btnDownPressed = false;
          break;
          //Button UP Pressed
          case 4:
          btnUPPressed   =  true;
          btnPos1Pressed = false;
          btnPos2Pressed = false;
          btnPos3Pressed = false;          
          btnDownPressed = false;
          btnSavePressed = false;
          break;
          //Button Down Pressed
          case 5:
          btnDownPressed =  true;             
          btnPos1Pressed = false;
          btnPos2Pressed = false;
          btnPos3Pressed = false;          
          btnUPPressed   = false;
          btnSavePressed = false;
          break;           
        }     
      }
      else 
      {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }      
    }
    
    // now we can ask the buttons if their state has changed 
    for (uint8_t b=0; b < 6; b++) 
    {
      if (buttons[b].justReleased()) 
      {              
        buttons[b].drawButton();    // draw normal
      }    
      if (buttons[b].justPressed()) 
      {
        buttons[b].drawButton(true);// draw invert!   
      }
    }
    
    if ((unsigned long)(currentMillis - previousMillis) >= 200)  //Measure every 200ms becaus else we get false readings some times 
    {
      newPosition = GetDistance(deskWidth); 
      previousMillis = currentMillis;
    }    
    
  if(btnSavePressed)  //if the save Buttons is pressed then  we press the other position buttons the possition is saved to the second button that we save;
  {           
   //Save Positiont1 
   if(btnPos1Pressed)
   {          
     memPosition[0] = GetDistance(deskWidth);
     btnPos1Pressed = false;
     btnSavePressed = false;
     DrawInfo("Saved Position 1: " + char(memPosition[0]) );     
   }   
   //Save Positiont2
   if(btnPos2Pressed)
   {     
     memPosition[1] = GetDistance(deskWidth);     
     btnPos2Pressed = false;
     btnSavePressed = false;
     DrawInfo("Saved Position 2: " + char(memPosition[1]) );     
   }   
   //Save Positiont3
   if(btnPos3Pressed)
   {    
     memPosition[2] = GetDistance(deskWidth);          
     btnPos3Pressed = false;
     btnSavePressed = false;  
     DrawInfo("Saved Position 3: " + char(memPosition[2]) );     
   }     
  }
  
  //Button UP Control  
  if(btnUPPressed && MODE == MANUAL)
  {
    DrawInfo("Auto UP");
    MODE = AUTOMATIC;
    targetPosition = 600; 
    oldPosition = GetDistance(deskWidth) + 0.60; //Adding some cm so it can sense that position is different and start moving
    btnUPPressed = false;
  }   
  //Button Down Control  
  if(btnDownPressed && MODE == MANUAL)
  {
    DrawInfo("Auto Down"); 
    MODE = AUTOMATIC;
    targetPosition = 2; 
    oldPosition = GetDistance(deskWidth) + 0.60; //Adding some cm so it can sense that position is different and start moving
    btnDownPressed = false;                      
  }
  
  if(btnSavePressed && MODE == AUTOMATIC)
  {
    stopActuator();
    MODE = MANUAL; 
    btnPos1Pressed = false;
    btnPos2Pressed = false;
    btnPos3Pressed = false;
    btnUPPressed   = false;
    btnDownPressed = false;
    btnSavePressed = false;
    DrawInfo("");   
  }

  //Button Position 1 Control 
  if(btnPos1Pressed && btnSavePressed) //So we do not move on saving position
  {        
  }

  if(btnPos1Pressed && !btnSavePressed)
  {
    DrawInfo("Moving to Pos 1"); 
    MODE = AUTOMATIC;
    targetPosition = memPosition[0];
    oldPosition = GetDistance(deskWidth) + 0.60;   //Adding some cm so it can sense that position is different and start moving    
    btnPos1Pressed = false;
  }
  
   //Button Position 2 Control 
  if(btnPos2Pressed && btnSavePressed) //So we do not move on saving position
  {    
  }

  if(btnPos2Pressed && !btnSavePressed)
  {
    DrawInfo("Moving to Pos 2"); 
    MODE = AUTOMATIC;
    targetPosition = memPosition[1];
    oldPosition = GetDistance(deskWidth) + 0.60;   //Adding some cm so it can sense that position is different and start moving      
    btnPos2Pressed = false;
  }
  
  //Button Position 3 Control 
  if(btnPos3Pressed && btnSavePressed) //So we do not move on saving position
  {    
  }

  if(btnPos3Pressed && !btnSavePressed)
  {
    DrawInfo("Moving to Pos 3"); 
    MODE = AUTOMATIC;
    targetPosition = memPosition[2];
    oldPosition = GetDistance(deskWidth) + 0.60;   //Adding some cm so it can sense that position is different and start moving    
    btnPos3Pressed = false;
  }
    
  //Control Actuator AUTOMATIC MODE
  if(MODE == AUTOMATIC && newPosition != targetPosition)   
  {     
    if(targetPosition < newPosition) 
    {
//      Serial.println("AUTO RETRACT");
//      Serial.println(newPosition);     
//      Serial.println(oldPosition-newPosition);      
      if((oldPosition-newPosition) < 1  && (oldPosition-newPosition) > - ERRORCORRECTION)   //this way we skip incorect readings 
      {                   
        retractActuator();
        MODE = AUTOMATIC;          
        oldPosition = newPosition;       
      }    
      DrawPosition(0,true);   
    }  
              
    if(targetPosition > newPosition) 
    {
//      Serial.println("AUTO EXTEND");
//      Serial.println(newPosition); 
//      Serial.println(newPosition - oldPosition);
          
      if((newPosition - oldPosition) < 1 && (newPosition - oldPosition) > - ERRORCORRECTION)  //this way we skip incorect readings 
      {         
       extendActuator();
       MODE = AUTOMATIC;       
       oldPosition = newPosition;
      }      
      DrawPosition(1,true); 
    } 
               
    if( (targetPosition == newPosition) || abs(targetPosition - newPosition) <= ACCURACY) 
    {
//    Serial.println("AUTO STOP");      
      MODE = MANUAL;       
      stopActuator();    
      DrawPosition(1,false); 
      DrawInfo("");        
    }  
  }    
}

//Move the Actuator UP
void extendActuator() 
{  
  digitalWrite(RELAY[0], HIGH);
  digitalWrite(RELAY[1], LOW);
}
//Move the Actuator Down
void retractActuator() 
{  
  digitalWrite(RELAY[0], LOW);
  digitalWrite(RELAY[1], HIGH);
}
void stopActuator() 
{  
  digitalWrite(RELAY[0], HIGH);
  digitalWrite(RELAY[1], HIGH);   
}

float GetDistance(float distanceCMToDeskUpper)
{
  float duration;
  float distanceCm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  float sp = (331.4 + (0.606 * dht.readTemperature()) + (0.0124 * dht.readHumidity())) / 10000; 
  distanceCm = (duration/2) * sp;
  return (distanceCm + distanceCMToDeskUpper);
}
