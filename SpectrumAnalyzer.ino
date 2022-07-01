/*
 Example program for the MD_MSGEQ7 library
 
 Reads the input value from the IC and displays a rolling table on the serial monitor.
*/

#include <MD_MSGEQ7.h>
#include <FastLED.h>
#include<SoftwareSerial.h>
SoftwareSerial mySerial(4,3);

// hardware pin definitions - change to suit circuit
#define DATA_PIN    A5
#define RESET_PIN   6
#define STROBE_PIN  5
#define NLED 140 //center 
#define NLED2 59 //corner
#define NLED3 4 //central dot, for bluetooth flag leds 
#define STEP 14
// frequency reading the IC data
#define READ_DELAY  40
#define COL_DELAY  2000
uint8_t data[MAX_BAND];
unsigned int temp[MAX_BAND];
uint8_t index;
uint8_t mode=0;
uint8_t color=0; //color variable for printcenter function
uint8_t color2=0;
uint8_t col=0; //color variable for all other functions 
uint8_t holdcol=random(1,6); //needed to not repeat the same color in a row during animations
uint8_t corner=0;
uint8_t holdanimation=0;
uint8_t Globalbrightness=100;
bool on=0;
bool ledon=0;
unsigned int Avgbass=0; 
unsigned int animationtime=0;


static unsigned long prevTime = 0; //time from last read from MSGEQ7
static unsigned long prevColor = 0; //time from last color change
static unsigned long prevanimation =0; //time from last animation switch
static unsigned long prevstep =0; //time for asincronous animations

//definition of Rainbow colors and led indexes
const unsigned int Rainbow[7]={0,32,64,96,128,160,200};
const uint8_t INDEXES[NLED]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,27,26,25,24,23,22,21,20,19,18,17,16,15,14,28,29,30,31,32,33,34,35,36,37,38,39,40,41,55,54,53,52,51,50,49,48,47,46,45,44,43,42,56,57,58,59,60,61,62,63,64,65,66,67,68,69,83,82,81,80,79,78,77,76,75,74,73,72,71,70,84,85,86,87,88,89,90,91,92,93,94,95,96,97,111,110,109,108,107,106,105,104,103,102,101,100,99,98,112,113,114,115,116,117,118,119,120,121,122,123,124,125,139,138,137,136,135,134,133,132,131,130,129,128,127,126};

//variabili per animazioni asincrone
uint8_t startindex=0;

CRGB leds[NLED];
CRGB leds2[NLED2];
CRGB leds3[NLED3];

MD_MSGEQ7 MSGEQ7(RESET_PIN, STROBE_PIN, DATA_PIN);


void setup() {
  MSGEQ7.begin();
  mySerial.begin(9600);
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, 8, GRB>(leds,NLED).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, 9, GRB>(leds2,NLED2).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, 10, GRB>(leds3,NLED3).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  SayHi();
  FastLED.setBrightness(Globalbrightness);
  for(int i=0;i<NLED3;i++){
     leds3[i]=CRGB::White;
  }
  FastLED.show();
}

void loop(){
  // only read every READ_DELAY milliseconds
  
  while(mode==0){ 
  if(millis()-prevstep>200 && ledon==1){
      WhiteStill();
      ledon=0;
  }
  if (millis() - prevTime >= READ_DELAY){
     ResetCenter();
     Read();
     PrintCenter();
     switch(corner){
      case 0:
              PulseOnBeat();
              break;
      case 1:
              ResetCorner();
              BarOnBeat(); 
              break;
      case 2:
              ResetCorner();
              ColorOnBeat();
              break;
      case 3:
              ColorStillOnBeat();
              break;
      case 4:
              RandomBar();
              break;
      case 5:
              Circle();
              break;
      case 6:
              ResetCorner();
              LongBarOnBeat();
              break;
      case 7:
              ResetCorner();
              Twobars();
              break;
      default:
              corner=random(0,8);
              break;
     }   
     FastLED.show();
     
     if(millis() - prevanimation>= animationtime){
        do{corner=random(0,8);}while(corner==holdanimation);
        if(corner==2)animationtime=random(3000,10000);
        //different random time for different kind of animations
        else if (corner==5)animationtime=random(8000,10000);
        else animationtime=random(10000,20000);
        holdanimation=corner;
        prevanimation=millis();     
     }    
  }
  ReadBT();
 }
 while(mode==1){
      if(millis()-prevstep>200 && ledon==1){
        WhiteStill();
        ledon=0;
      }
      ReadBT();
 }
  
while(mode==2){
      if(millis()-prevstep>200 && ledon==1){
          WhiteStill();
          ledon=0;
       }
      if (millis() - prevTime >= READ_DELAY){
       ResetCenter();
       Read();
       PrintCenter();
       FastLED.show();  
    }
    ReadBT();
}

 while(mode==3){
      ReadBT();
 }
}

void Read(){ 
      MSGEQ7.read();
    // Serial output
    for (uint8_t i=0; i<MAX_BAND; i++)
    {
      temp[i]=MSGEQ7.get(i);
      data[i]=Map(temp[i]);
      if(i<2)Avgbass+=temp[i];
    }
    Avgbass=Avgbass/2;
    prevTime = millis();
} 



//ANIMAZIONI CENTRO

//MusicReactive
void PrintCenter(){
    // Serial output 
    for (uint8_t i=0; i<MAX_BAND; i++){
      for(int k=0;k<data[i];k++){
        leds[INDEXES[index+k*STEP]]=CHSV(Rainbow[color],255,Globalbrightness);
        leds[INDEXES[index+1+k*STEP]]=CHSV(Rainbow[color],255,Globalbrightness);
      }
      color++;
      index+=2;
    }
    index=0;
    color=0;
}



//ANIMAZIONI CORNICE


void PulseOnBeat(){
      if(millis() - prevColor>=COL_DELAY*holdcol && data[0]>5){
        col=random(0,256);
        holdcol=random(1,6);
        prevColor=millis();
      }
      if(Avgbass>160)Avgbass=map(Avgbass,0,1024,0,255);
      else Avgbass=0;
      for(int i=0;i<NLED2;i++){
        leds2[i]=CHSV(col,255,Avgbass);
      } 
      Avgbass=0;
}  
void BarOnBeat(){
      uint8_t treshold1=data[0]*1.2;
      uint8_t treshold2=data[0]*1.8;  
      if(millis() - prevColor>=COL_DELAY*holdcol && data[0]>5){
        col=random(0,256);
        holdcol=random(1,6);
        prevColor=millis();
      }
      for(int i=9;i<treshold1+9;i++){
        leds2[i]=CHSV(col,255,Globalbrightness);
        leds2[i+30]=CHSV(col,255,Globalbrightness);
      }
      uint8_t k=50;
      for(int i=21;i<treshold2+21;i++){
        if(k==59)k=0;
        leds2[i]=CHSV(col,255,Globalbrightness);
        leds2[k]=CHSV(col,255,Globalbrightness);
        k++;
      }   
      Avgbass=0;
} 
void LongBarOnBeat(){
      uint8_t treshold=map(Avgbass,0,1024,0,60);
      if(treshold>NLED2)treshold=NLED2; 
      if(millis() - prevColor>=COL_DELAY*holdcol && data[0]>5){
        col=random(0,256);
        holdcol=random(1,3);
        prevColor=millis();
      }
      for(int i=0;i<treshold;i++){
        leds2[i]=CHSV(col,255,Globalbrightness);
      } 
      Avgbass=0;
}
void Twobars(){
      uint16_t tresholdH=(data[4]+data[5]+data[6])*1.2;
      if(tresholdH>26)tresholdH=30;
      uint8_t tresholdL=map(Avgbass,0,1024,0,29);
      if(tresholdL<2)tresholdL=0;
      else if(tresholdL>27)tresholdL=29;
      if(millis() - prevColor>=COL_DELAY*holdcol && data[0]>5){
        col=random(0,7);
        do{color2=random(0,7);}while(col==color2);
        holdcol=random(1,3);
        prevColor=millis();
      }
      for(int i=0;i<tresholdL;i++){
        leds2[i]=CHSV(Rainbow[col],255,Globalbrightness);
      }
      uint8_t k=58;
      while(k!=(NLED2-tresholdH-1)){
        leds2[k]=CHSV(Rainbow[color2],255,Globalbrightness);
        k--;
      } 
      Avgbass=0;
} 
void ColorOnBeat(){
    if(millis() - prevColor>=300 && (data[0]>5 || data[1]>5)){
        col=random(0,256);
        for(int i=0;i<NLED2;i++){
          leds2[i]=CHSV(col,255,Globalbrightness);
        }
        prevColor=millis();
      }    
}
void ColorStillOnBeat(){
    if(millis() - prevColor>=300 && (data[0]>5 || data[1]>5)){
        col=random(0,256);
        prevColor=millis();
    } 
    for(int i=0;i<NLED2;i++){
          leds2[i]=CHSV(col,255,Globalbrightness);
    }   
}
void RandomBar(){
   if(millis() - prevColor>=300 && (data[0]>5 || data[1]>5)){
        ResetCorner();
        holdcol=random(0,2);
        col=random(0,256);
        if(holdcol==0){
          for(int i=0;i<9;i++){
            leds2[i]=CHSV(col,255,Globalbrightness);
            leds2[i+50]=CHSV(col,255,Globalbrightness);
          }
          for(int i=21;i<38;i++){
            leds2[i]=CHSV(col,255,Globalbrightness);
          }
        }
        if(holdcol==1){
          for(int i=9;i<21;i++){
            leds2[i]=CHSV(col,255,Globalbrightness);
          }
          for(int i=38;i<51;i++){
            leds2[i]=CHSV(col,255,Globalbrightness);
          }
        }
        prevColor=millis();
   } 
}
void Circle(){
  if(millis() - prevColor>=300 && (data[0]>5 || data[1]>5)){
    col=random(0,256);
    prevColor=millis();
  }
  for(int i=0;i<startindex;i++){
    leds2[i]=CHSV(col,255,Globalbrightness);
  }
  if(startindex!=NLED2)startindex++;
  else{
    ResetCorner();  
    startindex=0;
  }
}  

uint8_t Map(uint16_t val){
      if(val<180) return 0;
      else if(val>=180 && val<237) return 1;
      else if(val>=237 && val<324) return 2;
      else if(val>=324 && val<411) return 3;
      else if(val>=411 && val<498) return 4;
      else if(val>=498 && val<585) return 5;
      else if(val>=585 && val<672) return 6;
      else if(val>=672 && val<759) return 7;
      else if(val>=759 && val<845) return 8;
      else if(val>=845 && val<933) return 9;
      else if(val>=933) return 10;   
}

void ResetCenter(){
  for(int i=0;i<NLED;i++){
      leds[i]=CRGB::Black;
  }
}

void ResetCorner(){
  for(int i=0;i<NLED2;i++){
      leds2[i]=CRGB::Black;
  }
}

void ReadBT(){
  if(mySerial.available()>0){
     Blueblink();
     int c=mySerial.read();
     BT(c);
  }
}
void BT(int k){
   bool exi=0;
   bool son=0;
   bool pressed=0;
   FastLED.show();
   switch(k){
    case 48: //Music Reactive Mode
       FastLED.clear();
       FastLED.show();
       mode=0;
       break;
    case 49: //Static Mode
       FastLED.clear();
       color=0;
       col=0;
       for(int i=0;i<NLED;i++){
        leds[i]=CHSV(color,255,Globalbrightness);
       }
       for(int i=0;i<NLED2;i++){
        leds2[i]=CHSV(col,255,Globalbrightness);
       }
       FastLED.show();
       mode=1;
       break;
    case 50: //Relax mode
       FastLED.clear();
       FastLED.setBrightness(Globalbrightness);
       for(int i=0;i<NLED2;i++){
         leds2[i]=CRGB::White;
       }
       FastLED.show();
       mode=2;
       break;
       
    case 51: //Strobo
       FastLED.clear();
       FastLED.show();
       FastLED.setBrightness(255);
       prevTime=millis();
       while(pressed==0){  
          if(millis()- prevTime>=20 && son==1){
                  FastLED.clear();
                  FastLED.show();
                  son=0;
                  prevTime=millis();
          } 
          else if(millis()- prevTime>=100 && son==0){ 
                  for(int i=0;i<NLED;i++){
                    leds[i]=CRGB::White;
                  }
                 for(int i=0;i<NLED2;i++){
                    leds2[i]=CRGB::White;
                  }
                  for(int i=0;i<NLED3;i++){
                    leds3[i]=CRGB::White;
                  }
                  FastLED.show();
                  son=1;
                  prevTime=millis();
          }           
          if (mySerial.available() > 0) {
              int d = mySerial.read();
              if(d==51){                      
                FastLED.setBrightness(Globalbrightness);
                FastLED.clear();
                for(int i=0;i<NLED3;i++){
                    leds3[i]=CRGB::White;
                }
                if(mode==1){
                  for(int i=0;i<NLED;i++){
                   leds[i]=CHSV(color,255,Globalbrightness);
                  }
                  for(int i=0;i<NLED2;i++){
                   leds2[i]=CHSV(col,255,Globalbrightness);
                  }                               
                }
                FastLED.show();
                pressed=1; 
              }
         }                                           
    }
    break;
    case 100: //change color of the center +++
           prevstep=millis()-150;
           while(exi==0){
              if (mySerial.available()> 0) {
                int c=mySerial.read();
                if(c==68)exi=1;
              }
              else if(millis()-prevstep>=150){
                 if(color>255)color=0;
                 else color+=5;
                 for(int i=0;i<NLED;i++){
                 leds[i]=CHSV(color,255,Globalbrightness);
                 }   
                 FastLED.show(); 
                 prevstep=millis();      
              }   
           }
    break;
    
    case(99): //change color of the center ---
    
           prevstep=millis()-150;
           while(exi==0){
              if (mySerial.available()> 0) {
                int c=mySerial.read();
                if(c==67)exi=1;
              }
              else if(millis()-prevstep>=150){
                 if(color<0)color=255;
                 else color-=5;
                 for(int i=0;i<NLED;i++){
                 leds[i]=CHSV(color,255,Globalbrightness);
                 }   
                 FastLED.show(); 
                 prevstep=millis();      
              }   
           }
           break;
    case(102): //change color corner+++

           prevstep=millis()-150;
           while(exi==0){
              if (mySerial.available()> 0) {
                int c=mySerial.read();
                if(c==70)exi=1;
              }
              else if(millis()-prevstep>=150){
                 if(col>255)col=0;
                 else col+=5;
                 for(int i=0;i<NLED2;i++){
                 leds2[i]=CHSV(col,255,Globalbrightness);
                 }   
                 FastLED.show(); 
                 prevstep=millis();      
              }   
           }
           break;
    case(101): //change color corner ---
          
           prevstep=millis()-150;
           
           while(exi==0){
              if (mySerial.available()> 0) {
                int c=mySerial.read();
                if(c==69)exi=1;
              }
              else if(millis()-prevstep>=150){
                 if(col<0)col=0;
                 else col-=5;
                 for(int i=0;i<NLED2;i++){
                 leds2[i]=CHSV(col,255,Globalbrightness);
                 }   
                 FastLED.show(); 
                 prevstep=millis();      
              }   
           }
           break;
    case(98): //Brightness up

           prevstep=millis()-100;
           while(exi==0){
             if (mySerial.available()> 0) {
                int c=mySerial.read();
                if(c==66)exi=1;
              }
             else if(millis()-prevstep>=80){
               Serial.println(Globalbrightness);
               if(Globalbrightness>255)Globalbrightness=50;
               else Globalbrightness+=5;
               FastLED.setBrightness(Globalbrightness);
               FastLED.show();
               prevstep=millis(); 
               }
             }
           break;
    case(97): //Brigthness down
    
           prevstep=millis()-100;
           while(exi==0){
               if (mySerial.available()> 0) {
                  int c=mySerial.read();
                  if (c==65)exi=1;
                }
               else if(millis()-prevstep>=80){
                 if(Globalbrightness<50)Globalbrightness=255;
                 else Globalbrightness-=5;
                 FastLED.setBrightness(Globalbrightness);
                 FastLED.show();
                 prevstep=millis(); 
               }
            }
            break;
    case(103): //ON/OFF
       if(on==0){
        SayHi();
       }
       else if(on==1){
        FastLED.clear();
        for(int i=0; i<NLED3;i++){
            leds3[i]=CRGB::Red;
        }
        FastLED.show(); 
        on=0;
        mode=3;
       }
       break;
    default:
    break;
   }
}
void SayHi(){
      FastLED.clear();
      FastLED.show();
      for(int i=0;i<3;i++){
          for(int i=0; i<NLED2;i++){
            leds2[i]=CRGB::White;
          }
          for(int i=0; i<NLED3;i++){
            leds3[i]=CRGB::White;
          }
          leds[3]=CRGB::White;leds[24]=CRGB::White;leds[31]=CRGB::White;leds[52]=CRGB::White;leds[59]=CRGB::White;leds[80]=CRGB::White;leds[87]=CRGB::White;leds[108]=CRGB::White;leds[136]=CRGB::White;
          leds[60]=CRGB::White;leds[61]=CRGB::White;leds[62]=CRGB::White;leds[6]=CRGB::White;leds[21]=CRGB::White;leds[34]=CRGB::White;leds[49]=CRGB::White;leds[77]=CRGB::White;leds[90]=CRGB::White;
          leds[105]=CRGB::White;leds[118]=CRGB::White;leds[115]=CRGB::White;leds[133]=CRGB::White;leds[8]=CRGB::White;leds[9]=CRGB::White;leds[19]=CRGB::White;leds[18]=CRGB::White;leds[36]=CRGB::White;
          leds[37]=CRGB::White;leds[47]=CRGB::White;leds[46]=CRGB::White;leds[64]=CRGB::White;leds[65]=CRGB::White;leds[75]=CRGB::White;leds[74]=CRGB::White;leds[92]=CRGB::White;leds[93]=CRGB::White;
          leds[120]=CRGB::White;leds[121]=CRGB::White;leds[131]=CRGB::White;leds[130]=CRGB::White;leds[79]=CRGB::White;leds[78]=CRGB::White;
          FastLED.show();
          delay(500);
          FastLED.clear();
          FastLED.show();
          delay(250);
     }
     FastLED.setBrightness(Globalbrightness);
     color=random(0,256);
     col=color;
       for(int i=0;i<NLED;i++){
        leds[i]=CHSV(color,255,Globalbrightness);
       }
       for(int i=0;i<NLED2;i++){
        leds2[i]=CHSV(col,255,Globalbrightness);
       }
     FastLED.show();
     on=1;
     mode=1;
}
void Blueblink(){
   ledon=1;
   prevstep=millis();
   for(int i=0;i<NLED3;i++){
    leds3[i]=CRGB::Blue;
   }
   FastLED.show();
}
void WhiteStill(){
   for(int i=0;i<NLED3;i++){
    leds3[i]=CRGB::White;
   }
   FastLED.show(); 
}
