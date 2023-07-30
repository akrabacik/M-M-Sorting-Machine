#include <Servo.h>

Servo tamturservo;

int pos = 0;

void setup() {
  tamturservo.attach(3); 
  //tamturservo.write(90);
}

void loop() {
  tamturservo.write(90);
  delay(1200);
  tamturservo.write(180);
  delay(200);
  tamturservo.write(0);
  delay(600);

}
