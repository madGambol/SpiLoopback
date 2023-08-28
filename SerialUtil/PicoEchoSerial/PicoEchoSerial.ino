

#include <Serial.h>

UART com( p0, p1 );

char * pClear = "\x1b\x5b2J";

void setup() 
{
  Serial.begin( 115200 );
  com.   begin( 115200 );

  while (!Serial) 
  {
    delay(1);
  }
  
  Serial.println("done delaying");

  while(!com) 
  {
    delay(1);
  }
  Serial.println("com ready");

  //Serial.write(pClear);

  //Serial.write("\fsetup\n\r");

  com.write("!01234567890123456789!\n");

  pinMode(LED_BUILTIN, OUTPUT);

}

void toggleLed(void)
{
  static bool bOn = false;

  if (bOn)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else 
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  bOn = !bOn;
}

uint32_t charCount = 0;
uint32_t oldCount  = 0xffffffff;

uint32_t loopCount = 0;

bool bMilli = true;

void loop() 
{
  if (com.available())
  {
    int ch = com.read();
    Serial.write(ch);
    ++charCount;
  }

  if (Serial.available())
  {
    int ch = Serial.read();
    com.write( ch );
  }

  if (((millis() % 1000) == 0) && bMilli )
  {
    bMilli = false;

    toggleLed();

    if ((oldCount  != charCount) || (millis() % 10000) == 0)
    {
      //Serial.write("Count = ");
      //Serial.print( charCount );
      //Serial.write( ", loopcount = ");
      //Serial.println( loopCount );

      ++loopCount;      

      oldCount  = charCount;
    }
  }
  else if ((millis() % 1000) == 50)
  {
    bMilli = true;
  }
}
