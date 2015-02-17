/*
 * http://macetech.com/blog/node/54
 */
 
 
int datapin  = 10; // DI
int latchpin = 11; // LI
int enablepin = 12; // EI
int clockpin = 13; // CI
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

#define MAX_LEDS 15

unsigned long leds[ MAX_LEDS ];

void setup() {
   pinMode( 7, OUTPUT );
   pinMode(datapin, OUTPUT);
   pinMode(latchpin, OUTPUT);
   pinMode(enablepin, OUTPUT);
   pinMode(clockpin, OUTPUT);

   digitalWrite(latchpin, LOW);
   digitalWrite(enablepin, LOW);

   SB_CommandMode = B01; // Write to current control registers
   SB_RedCommand = 0; // min current
   SB_GreenCommand = 0; // min current
   SB_BlueCommand = 0; // min current
   SB_SendPacket();
   
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ] = 0;
   
}

void setColor( int r, int g, int b, long msDelay )
  {

   SB_CommandMode = 0; // Write to PWM control registers
   SB_RedCommand = r; // Maximum red
   SB_GreenCommand = g; // Minimum green
   SB_BlueCommand = b; // Minimum blue
   SB_SendPacket();

   delay(msDelay);
  }
  
void SB_SendPacket() {
   SB_CommandPacket = SB_CommandMode & B11;
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

   delay(5); // adjustment may be necessary depending on chain length
   digitalWrite(latchpin,HIGH); // latch data into registers
   delay(6); // adjustment may be necessary depending on chain length
   digitalWrite(latchpin,LOW);

}

void setColors( long msDelay )
  {
    int  count = MAX_LEDS; //sizeof( leds ) / sizeof( unsigned long );
    unsigned long color;
    
//    digitalWrite( latchpin, LOW );
    
    for ( int i = 0; i < count; i++ )
      {
      unsigned long command  = 0;
      
      color    = leds[ i ];
      byte red    = ( color & 0x00ff0000 ) >> 16;
      byte green  = ( color & 0x0000ff00 ) >> 8;
      byte blue   = ( color & 0x000000ff );
      
      command  = blue & 0x3ff;
      command  = ( command << 10 ) | ( red & 0x3ff );
      command  = ( command << 10 ) | ( green & 0x3ff );

      shiftOut( datapin, clockpin, MSBFIRST, ( command >> 24 ) & 0xff );
      shiftOut( datapin, clockpin, MSBFIRST, ( command >> 16 ) & 0xff );
      shiftOut( datapin, clockpin, MSBFIRST, ( command >> 8 ) & 0xff );
      shiftOut( datapin, clockpin, MSBFIRST, command & 0xff );
      
      }

    delay( 5 );
    digitalWrite( latchpin, HIGH );
    delay( 6 );
    digitalWrite( latchpin, LOW );
      

    if ( msDelay )    
      delay( msDelay );
  }
  
void flash( int int1, int int2 ) {
  for ( int i = 0; i < MAX_LEDS; i++ )
    leds[ i ] = 0;
  setColors( int1 );
  for ( int i = 0; i < MAX_LEDS; i++ )
    leds[ i ] = 0x00ffffff;
  setColors( int2 );
}

void chaser( unsigned long color1, unsigned long color2, unsigned long color3, int interval ) {
 int  i = 0;
 
 for ( ; i < MAX_LEDS / 3; i++ )
  leds[ i ] = color1;
 
 for ( ; i <  2 * MAX_LEDS / 3; i++ )
   leds[ i ] = color2;
   
 for ( ; i < MAX_LEDS; i++ )
   leds[ i ] = color3;
   
 setColors( interval );
  
}

void setLeds( long color )
  {
    for ( int i = 0; i < MAX_LEDS; i++ )
      leds[ i ] = color;
  }
  
void blankem()
  {
  setLeds( 0 );
  }

void lightOneLevel( int level, unsigned long color )
  {
    leds[ level ] = color;
    leds[ level + 5 ] = color;
    leds[ level + 10 ] = color;
  }
  
void policeLights()
  {
    
  for ( int i = 0; i < 10; i++ )
    {
    lightOneLevel( 0, 0xff0000 );
    lightOneLevel( 1, 0x0000ff );
    lightOneLevel( 2, 0xff0000 );
    lightOneLevel( 3, 0x0000ff );
    lightOneLevel( 4, 0xff0000 );
    setColors( 100 );
    for ( int i = 0; i < 2; i++ )
      {
      digitalWrite( enablepin, HIGH );
      delay( 50 );
      digitalWrite( enablepin, LOW );
      delay( 50 );
      }

    lightOneLevel( 0, 0x0000ff );
    lightOneLevel( 1, 0xff0000 );
    lightOneLevel( 2, 0x0000ff );
    lightOneLevel( 3, 0xff0000 );
    lightOneLevel( 4, 0x0000ff );
    setColors( 100 );
    
    for ( int i = 0; i < 2; i++ )
      {
      digitalWrite( enablepin, HIGH );
      delay( 50 );
      digitalWrite( enablepin, LOW );
      delay( 50 );
      }
    }
  }
  
void growUp()
  {
  blankem();
  setColors( 0 );
  
  for ( int h = 0; h < 5; h++ )
    {
      lightOneLevel( h, 0x00ffffff );
    setColors( 100 );
    blankem();
    }
  }
  
void glow( long interval )
  {
  long color;
  blankem();
  
  for ( int i = 0; i <= 0x3f; i += 4 )
    {
    color = ( (long) i << 16 ) | ( i << 8 ) | i;
    setLeds( color );
    setColors( interval );
    }

  delay( 50 );
  
  for ( int i = 0x3f; i >= 0; i -= 4 )
    {
    color = ( (long) i << 16 ) | ( i << 8 ) | i;
    setLeds( color );
    setColors( interval );
    }
  setLeds( 0 );
  setColors( interval );
  delay( 25 );
  }

void blueGlow( long interval )
  {
  long color;
  blankem();
  
  for ( int i = 0; i <= 0x3f; i += 4 )
    {
    color = i;
    setLeds( color );
    setColors( interval );
    }

  delay( 50 );
  
  for ( int i = 0x3f; i >= 0; i -= 4 )
    {
    color = i;
    setLeds( color );
    setColors( interval );
    }
  setLeds( 0 );
  setColors( interval );
  delay( 25 );
  }

void redGlow( long interval )
  {
  long color;
  blankem();
  
  for ( int i = 0; i <= 0x3f; i += 4 )
    {
    color = ( (long) i ) << 16;
    setLeds( color );
    setColors( interval );
    }

  delay( 50 );
  
  for ( int i = 0x3f; i >= 0; i -= 4 )
    {
    color = ( (long) i ) << 16;
    setLeds( color );
    setColors( interval );
    }
  setLeds( 0 );
  setColors( interval );
  delay( 25 );
  }

void greenGlow( long interval )
  {
  long color;
  blankem();
  
  for ( int i = 0; i <= 0x3f; i += 4 )
    {
    color = ( (long) i ) << 8;
    setLeds( color );
    setColors( interval );
    }

  delay( 50 );
  
  for ( int i = 0x3f; i >= 0; i -= 4 )
    {
    color = ( (long) i ) << 8;
    setLeds( color );
    setColors( interval );
    }
  setLeds( 0 );
  setColors( interval );
  delay( 25 );
  }

void loop() {
  
  for ( int i = 0; i < MAX_LEDS; i++ )
    {
   SB_CommandMode = B00; // Write to PWM control registers
   SB_RedCommand = 0; // Maximum red
   SB_GreenCommand = 0; // Minimum green
   SB_BlueCommand = 0; // Minimum blue
   SB_SendPacket();

   delay( 0 );
    }
    
/* why doesn't this work reliably???    
  digitalWrite( enablepin, HIGH );
  digitalWrite( datapin, LOW );
  digitalWrite( clockpin, LOW );
  for ( int i = 0; i < ( MAX_LEDS + 10 ) * 32; i++ )
    {
    delay( 2 );
    digitalWrite( clockpin, HIGH );
    delay( 2 );
    digitalWrite( clockpin, LOW );
    }
  delay( 25 );
  digitalWrite( latchpin, HIGH );
  delay( 25 );
  digitalWrite( latchpin, LOW );
  digitalWrite( enablepin, LOW );
  delay( 500 );
*/  
  
/*
  // flash lights using enable pin
  for ( int i = 0; i < 4; i++ )
    {
    digitalWrite( enablepin, HIGH );
    delay( 500 );
    digitalWrite( enablepin, LOW );
    delay( 500 );
    }  
*/  
  
/*   
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x000000ff;
   setColors( 100 );
   
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x0000ff00;
   setColors( 100 );

   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x00ff0000;
   setColors( 100 );

   for ( int i = 0; i < 10; i++ )
     flash( 200, 50 );
     
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ] = 0;
*/
/*
   for ( int j = 0; j < 3; j++ )
      {
      chaser( 0x00ff0000, 0x000000ff, 0, 200 );     
      chaser( 0, 0x00ff0000, 0x000000ff, 200 );     
      chaser( 0x000000ff, 0, 0x00ff0000, 200 );     
      }
      
   for ( int j = 0; j < 10; j++ )
     {
      chaser( 0x00000007, 0x00000001, 0x3f, 100 );     
      chaser( 0x00000001, 0x0000003f, 0x07, 100 );     
      chaser( 0x0000003f, 0x00000007, 0x01, 100 );     
     }
*/

  for ( int i = 0; i < 3; i++ )
    growUp();

/*
  for ( int i = 0; i < 5; i++ )
    glow( 0 );  
    
  delay( 2000 );
  for ( int i = 0; i < 5; i++ )
    greenGlow( 0 );  
    
  delay( 2000 );
*/
  
  blueGlow( 0 );
  redGlow( 0 );
  blueGlow( 0 );
  redGlow( 0 );
  
  policeLights();
  
//   for ( int i = 0; i < MAX_LEDS; i++ )
//     {
//     unsigned long tmp;
//     tmp  = leds[ 0 ];
//     for ( int j = 0; j < MAX_LEDS - 2; j++ )
//       leds[ j ] = leds[ j + 1 ];
//     leds[ MAX_LEDS - 1 ] = tmp;
//     setColors( 500 );
//     }
}
