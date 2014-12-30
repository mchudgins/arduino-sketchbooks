#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
// the sensor communicates using SPI, so include the library:
#include <SPI.h>

Adafruit_7segment matrix = Adafruit_7segment();
int CS = 1;
int dac = 0x0ffa;

void setup()
  {
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
  
  }

void loop()
  {
/*  
  if ( CS == 1 )
    {
    digitalWrite( 4, LOW );
    CS = 0;
    } 
  else
    {
    digitalWrite( 4, HIGH );
    CS = 1;
    }

  matrix.print( CS, DEC );
  matrix.writeDisplay();
  */
  matrix.print( dac >> 2, HEX );
  matrix.writeDisplay();

  digitalWrite( 4, LOW );
  SPI.transfer( dac >> 8 );
  SPI.transfer ( dac );
  digitalWrite( 4, HIGH );
  
  dac -= 4;
  
  if ( dac == 0 )
    dac = 0x0ffa;  
    
  if ( dac >= 0x0ffa )
    dac = 0;

  delay( 2000 );
  }



