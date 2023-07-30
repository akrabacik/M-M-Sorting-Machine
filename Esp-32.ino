#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>


#define SERVO_1    12

const byte rxPin = 0;
const byte txPin = 1;
unsigned char cr = 0;
unsigned char cb = 0;
unsigned char cg = 0;
unsigned char co = 0;
unsigned char cy = 0; 
unsigned char cw = 0;
unsigned char ct = 0;

Servo myservo;

unsigned long myTime;

char str[16];

unsigned long servoTime;
bool flag = false;


LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial mySerial (rxPin, txPin); 

void setup() 
{
  mySerial.begin(115200);
  Serial.begin(9600);
  Serial.print("Started");
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.clear();
}

void loop() 
{
  if(mySerial.available())
  {
    String msg = mySerial.readString();
    Serial.println(msg);
    if(msg == "R"){
      cr++;
      ct++;
      myservo.write(60);
      servoTime = milis();
      flag = true;

    }
    else if(msg == "G"){
      cg++;
      ct++;
    }
    else if(msg == "B"){
      cb++;
      ct++;
    }
    else if(msg == "O"){
      co++;
      ct++;
    }
    else if(msg == "Y"){
      cy++;
      ct++;
    }
    else if(msg == "W"){
      cw++;
      ct++;
    }
    
    if(flag && (milis()-servoTime)>1000){
      myservo.write(0);
      flag = false;
    }
    
  }
  //Serial.println(String(cr)+" "+String(cg)+" "+String(cb)+" "+String(co)+" "+String(cy)+" "+String(cw)+" "+String(ct));
  if((millis()-mytime)>500){
    myTime = millis(); //myTime += 500;
    lcd.setCursor(0, 0);
    sprintf(str, "R%02d G%02d B%02d O%02d",cr,cg,cb,co);
    lcd.print(str);

    lcd.setCursor(0, 1);
    sprintf(str, "Y%02d W%02d T%02d",cy,cw,ct);
    lcd.print(str);
  }
}