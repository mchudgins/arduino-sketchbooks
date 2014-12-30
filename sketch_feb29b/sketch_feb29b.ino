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

void SB_SendPacket() {
   SB_CommandPacket = SB_CommandMode & B11;
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
   SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
   shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

   delay(5); // adjustment may be necessary depending on chain length
   digitalWrite(latchpin,HIGH); // latch data into registers
   delay(6); // adjustment may be necessary depending on chain length
   digitalWrite(latchpin,LOW);

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
  
void setColors( long msDelay )
  {
    int  count = MAX_LEDS; //sizeof( leds ) / sizeof( unsigned long );
    
    for ( int i = 0; i < count; i++ )
      {
      unsigned long command  = 0;
      
      byte red    = ( leds[ i ] & 0x00ff0000 ) >> 16;
      byte green  = ( leds[ i ] & 0x0000ff00 ) >> 8;
      byte blue   = ( leds[ i ] & 0x000000ff );
      
      SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
      SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
      SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);
      command  = blue;
      command  = ( command << 10 ) | red;
      command  = ( command << 10 ) | green;
      
      shiftOut( datapin, clockpin, MSBFIRST, command >> 24 );
      shiftOut( datapin, clockpin, MSBFIRST, command >> 16 );
      shiftOut( datapin, clockpin, MSBFIRST, command >> 8 );
      shiftOut( datapin, clockpin, MSBFIRST, command );
      
      }
      
    delay( 10 );
    digitalWrite( latchpin, HIGH );
    delay( 10 );
    digitalWrite( latchpin, LOW );
    
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

void loop() {
  
/*   SB_CommandMode = B00; // Write to PWM control registers
   SB_RedCommand = 256; // Maximum red
   SB_GreenCommand = 0; // Minimum green
   SB_BlueCommand = 0; // Minimum blue
   SB_SendPacket();

   delay(50);
 */

//   setColor( 0, 0, 0, 1000 );
   
   leds[ 0 ] = 0x00ff0000;
   leds[ 1 ] = 0x0000ff00;
   leds[ 2 ] = 0x000000ff;
   leds[ 3 ] = 0x000f0f0f;
   leds[ 4 ] = 0x00000000;
   setColors( 5000 );
   
//   for ( int i = 0; i < MAX_LEDS; i++ )
//     leds[ i ]  = 0;
//   leds[ 0 ] = 0X00ffffff;
//   leds[ 1 ] = 0X00ffffff;
//   leds[ 2 ] = 0X00ffffff;
//   leds[ 3 ] = 0X00ffffff;
//   leds[ 4 ] = 0X00ffffff;
//   setColors( 200 );

   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x000000ff;
   setColors( 2000 );
   
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x0000ff00;
   setColors( 2000 );

   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ]  = 0x00ff0000;
   setColors( 2000 );
   
   for ( int i = 0; i < 10; i++ )
     flash( 200, 50 );
     
   for ( int i = 0; i < MAX_LEDS; i++ )
     leds[ i ] = 0;
     
   for ( int j = 0; j < 20; j++ )
      {
      chaser( 0x00ff0000, 0x000000ff, 0, 200 );     
      chaser( 0, 0x00ff0000, 0x000000ff, 200 );     
      chaser( 0x000000ff, 0, 0x00ff0000, 200 );     
      }
      
//   for ( int i = 0; i < MAX_LEDS; i++ )
//     {
//     unsigned long tmp;
//     tmp  = leds[ 0 ];
//     for ( int j = 0; j < MAX_LEDS - 2; j++ )
//       leds[ j ] = leds[ j + 1 ];
//     leds[ MAX_LEDS - 1 ] = tmp;
//     setColors( 500 );
//     }
   
   leds[ 0 ] = 0x000f0000;
   leds[ 1 ] = 0x00000f00;
   leds[ 2 ] = 0x0000000f;
   leds[ 3 ] = 0x00707070;
   leds[ 4 ] = 0x00f0f0f0;
   setColors( 1000 );
   
/*
   for ( int i = 0; i < 1024; i += 32 )
     {
     setColor( i, 0, 0, 75 );
     }
   for ( int i = 1023; i > 0 ; i -= 32 )
     setColor( i, 0, 0, 75 );
   for ( int i = 0; i < 15; i++ )
     {
     setColor( i * 64, 0, 0, 50 );
     }
   for ( int i = 15; i ; i-- )
     setColor( i * 64, 0, 0, 50 );

   for ( int i = 0; i < 15; i++ )
     {
     setColor( 0, i * 64, 0, 50 );
     }
   for ( int i = 15; i ; i-- )
     setColor( 0, i * 64, 0, 50 );
     
   for ( int i = 0; i < 15; i++ )
     {
     setColor( 0, 0, i * 64, 50 );
     }
   for ( int i = 15; i ; i-- )
     setColor( 0, 0, i * 64, 50 );

//   digitalWrite( 7, LOW );
   setColor( 500, 0, 20, 500 );
//   digitalWrite( 7, HIGH );
   setColor( 300, 0, 256, 500 );
//   digitalWrite( 7, LOW );
   setColor( 200, 0, 500, 500 );
//   digitalWrite( 7, HIGH );

//   setColor( 1023, 1023, 1023, 10);
//   delay( 500 );

  for ( int i = 0; i < 10; i++ )
    {  
   digitalWrite(enablepin, HIGH );
   delay( 50 );
   digitalWrite(enablepin, LOW );
   delay( 50 );
    }
  */
}
