#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <Wire.h>
#include <AdaEncoder.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

#define MAX_SAMPLES 3
#define a_PINA 10
#define a_PINB 9

int8_t clicks=0;
char id=0;
int value = 0;

Adafruit_7segment matrix = Adafruit_7segment();
int CS = 1;
int dac = 0x0ffa;

int loopcount = 0;
int adsum = 0;

void setup()
  {
  unsigned char low;
  unsigned char hi;
  
  digitalWrite( 4, HIGH );

  //start serial connection
  Serial.begin( 9600 );
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode( 4, OUTPUT );
 
  matrix.begin(0x70);

  // start the SPI library:
  SPI.setDataMode( SPI_MODE3 );
  SPI.setBitOrder( MSBFIRST );
  
  SPI.begin();
  
  AdaEncoder::addEncoder( 'a', a_PINA, a_PINB );
  matrix.print( value, HEX );
  matrix.writeDisplay();
  
  // setup Analog reference to default of 5V
//  analogReference( DEFAULT );
  analogReference( EXTERNAL );
  
  Serial.println( "Mercury, 0.9.0" );
  Serial.print( "Status Reg:  " );
  Serial.println( SREG, HEX );
  
  Serial.print( "SMCR:  " );
  Serial.println( SMCR, HEX );
  
  Serial.print( "EICRA:  " );
  Serial.println( EICRA, HEX );
  Serial.print( "EIMSK:  " );
  Serial.println( EIMSK, HEX );
  
  Serial.print( "TCCR0A:  " );
  Serial.println( TCCR0A, HEX );
  Serial.print( "TIMSK0:  " );
  Serial.println( TIMSK0, HEX );
  Serial.print( "TIFR0:  " );
  Serial.println( TIFR0, HEX );
  Serial.print( "TCNT0:  " );
  Serial.println( TCNT0, HEX );
  
  Serial.print( "TCCR0B:  " );
  Serial.println( TCCR0B, HEX );
  
  Serial.print( "TCCR1A:  " );
  Serial.println( TCCR1A, HEX );
  Serial.print( "TCCR1B:  " );
  Serial.println( TCCR1B, HEX );
  Serial.print( "TIMSK1:  " );
  Serial.println( TIMSK1, HEX );
  Serial.print( "TCNT1:  " );
  low  = TCNT1L;
  hi  = TCNT1H;
  Serial.print( hi, HEX );
  Serial.println( low, HEX ); 
  }

void loop()
{
  int ad;
  
  delay( 10 );
  
/*  
  ad = analogRead( 1 );
  
  adsum += ( ad & 0xffff );
  if ( ++loopcount == MAX_SAMPLES )
    {
    ad = adsum / MAX_SAMPLES;
    float voltage = ad * 5000.0 / 1024;
    matrix.print( (int) voltage, DEC );
    matrix.writeDisplay();
    
    loopcount = 0;
    adsum = 0;
    }
*/

  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie(&clicks, &id);
  if (thisEncoder != NULL) {
    Serial.print(id); Serial.print(':');
    if (clicks > 0) {
      Serial.println(" CW");
    }
    if (clicks < 0) {
       Serial.println(" CCW");
    }

  value += clicks;  
  matrix.print( value, HEX );
  matrix.writeDisplay();
  }
  
}



