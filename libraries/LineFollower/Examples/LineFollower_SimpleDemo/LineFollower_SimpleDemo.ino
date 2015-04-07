#include "LineFollower.h"

int linePins[] = {28, 29, 30, 31, 32, 34, 35, 36}; 
int const LINE_EN = 37;
LineFollower cyLine(linePins, LINE_EN);

void setup()
{
  cyLine.enable();
  Serial.begin(9600);
  delay(2000);
}

void loop()
{
   Serial.println(cyLine.measure(), BIN);
   delay(1000);
}
