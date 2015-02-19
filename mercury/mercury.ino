/*
  This sketch uses an Arduino to drive a relay and a led strip.
 */
#include <PinChangeInt.h> // necessary otherwise we get undefined reference errors.
#include <Wire.h>
#ifdef  ENCODER
#include <AdaEncoder.h>
#endif
#ifdef  MATRIX
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#endif

// the sensor communicates using SPI, so include the library:
#include <SPI.h>

#define MAX_SAMPLES 3

#ifdef  ENCODER
#define a_PINA 10
#define a_PINB 9
#define a_SW 8
#endif

#define  RED_PIN  3
#define  GRN_PIN  5
#define  BLU_PIN  6
#define  MAX_PWM  64

#ifdef  ENCODER
int8_t clicks=0;
char id=0;
#endif

#ifdef  MATRIX
volatile int value = 0;

Adafruit_7segment matrix = Adafruit_7segment();
char brightness  = 15;
#endif

int CS = 1;
int dac = 0x0ffa;

int loopcount = 0;
int adsum = 0;

volatile unsigned char fTimer0  = 0;
volatile unsigned char fTimer1  = 0;
volatile unsigned char fTimer2  = 0;

volatile char  szInputBuf[ 8 ];
volatile int  iInputBuf  = 0;
volatile char fCmdAvailable = 0;

char cycle = 0;
char redPWM = 8;
char grnPWM = 4;
char bluPWM = 0;

boolean  f4    = true;

void    (*timer0Routine)();

/*
 * fade the leds as white light up and down
 */
 
char  iFade;
boolean fadeDown()
  {
    if ( iFade++ < MAX_PWM )
      {
      --redPWM;
      --grnPWM;
      --bluPWM;
      return( false );
      }
    return( true );
  }
boolean fadeUp()
  {
    if ( iFade++ < MAX_PWM )
      {
      ++redPWM;
      ++grnPWM;
      ++bluPWM;
      return( false );
      }
    return( true );
  }
boolean  (*currentFade)()  = fadeDown;  
void continuousFade()
  {
      if ( (*currentFade)() )
        {
        if ( currentFade == fadeDown )
            {
            iFade  = 0;
            currentFade = fadeUp;
            redPWM = 0;
            grnPWM = 0;
            bluPWM = 0;
            }
        else
          {
          iFade  = 0;
          currentFade = fadeDown;
          redPWM = MAX_PWM;
          grnPWM = MAX_PWM;
          bluPWM = MAX_PWM;
          }
        }
  }
  
/*
 * Bright green, fade to black
 */
unsigned char iGreenToBlack  = 0;
void greenToBlack()
  {
    if ( bluPWM )
      --bluPWM;
    if ( redPWM )
      --redPWM;
      
    if ( iGreenToBlack++ < ( MAX_PWM << 1 ) )
      {
      if ( iGreenToBlack & 1 )
        --grnPWM;
      }
    else
      timer0Routine = NULL;
  }
 
/*
 * Bright red, fade to black
 */
unsigned char iRedToBlack  = 0;
void redToBlack()
  {
    if ( bluPWM )
      --bluPWM;
    if ( grnPWM )
      --grnPWM;
      
    if ( iRedToBlack++ < ( MAX_PWM << 1 ) )
      {
      if ( iRedToBlack & 1 )
        --redPWM;
      }
    else
      timer0Routine = NULL;
  }
 
/*
 *  flash the led's like red/blue police lights
 */
 
unsigned char  iPolice = 0;
boolean policeDown()
  {
    if ( iPolice++ < ( MAX_PWM << 1 ) )
      {
      if ( iPolice & 1 )
        --redPWM;
      return( false );
      }
    return( true );
  }
boolean policeUp()
  {
    if ( iPolice++ < ( MAX_PWM << 1 ) )
      {
      if ( iPolice & 1 )
        ++redPWM;
      return( false );
      }
    return( true );
  }
char iPoliceAlternate  = 0;
char iPoliceAlternateState = 0;
boolean policeAlternate()
  {
    if ( iPoliceAlternate < 24 )
      {
      switch ( iPoliceAlternateState )
        {
          case 0:
              if ( iPolice++ > 8 )
                {
                iPoliceAlternateState = 1;
                iPoliceAlternate++;
                iPolice = 0;
                redPWM  = 0;
                bluPWM  = MAX_PWM;
                }
                break;
                
           case 1:
              if ( iPolice++ > 8 )
                {
                iPoliceAlternateState = 2;
                iPoliceAlternate++;
                iPolice = 0;
                redPWM  = 0;
                bluPWM  = 0;
                }
              break;

           case 2:
              if ( iPolice++ > 8 )
                {
                iPoliceAlternateState = 3;
                iPoliceAlternate++;
                iPolice = 0;
                redPWM  = MAX_PWM;
                bluPWM  = 0;
                }
              break;

           case 3:
              if ( iPolice++ > 8 )
                {
                iPoliceAlternateState = 0;
                iPoliceAlternate++;
                iPolice = 0;
                redPWM  = 0;
                bluPWM  = 0;
                }
              break;
        }
      return( false );
      }
  return( true );
  }
unsigned char currentPoliceState = 0;  
boolean  (*currentPolice)()  = policeDown;  
void policeLights()
  {
      if ( (*currentPolice)() )
        {
        switch ( currentPoliceState )
            {
            case 0:
              iPolice  = 0;
              currentPoliceState = 1;
              currentPolice = policeUp;
              redPWM = 0;
              grnPWM = 0;
              bluPWM = 0;
              break;
            
            case 1:
              iPolice  = 0;
              currentPoliceState = 2;
              currentPolice = policeDown;
              redPWM = MAX_PWM;
              grnPWM = 0;
              bluPWM = 0;
              break;
              
            case 2:
              iPolice  = 0;
              currentPoliceState = 3;
              currentPolice = policeUp;
              redPWM = 0;
              grnPWM = 0;
              bluPWM = 0;
              break;
              
            case 3:
              iPolice  = 0;
              currentPoliceState = 4;
              currentPolice = policeDown;
              redPWM = MAX_PWM;
              grnPWM = 0;
              bluPWM = 0;
              break;

            case 4:
              iPolice  = 0;
              currentPoliceState = 5;
              currentPolice = policeUp;
              redPWM = 0;
              grnPWM = 0;
              bluPWM = 0;
              break;

            case 5:
              iPolice  = 0;
              currentPolice = policeAlternate;
              iPoliceAlternate = 0;
              iPoliceAlternateState = 0;
              currentPoliceState = 0;
              redPWM = 0;
              grnPWM = 0;
              bluPWM = 0;
              break;
            }
            
#if 0            
        if ( currentPolice == policeDown )
            {
            iPolice  = 0;
            currentPolice = policeAlternate;
            iPoliceAlternate = 0;
            iPoliceAlternateState = 0;
            currentPolice = policeAlternate;
            redPWM = 0;
            grnPWM = 0;
            bluPWM = 0;
            }
        else if ( currentPolice == policeUp )
            {
            iPolice  = 0;
            currentPolice = policeAlternate;
            iPoliceAlternate = 0;
            iPoliceAlternateState = 0;
            }
        else
          {
          iPolice  = 0;
          currentPolice = policeDown;
          redPWM = MAX_PWM;
          grnPWM = 0;
          bluPWM = 0;
          }
#endif          
        }
  }
 
  

void setupTimers()
  {
  cli();//stop interrupts

#if 0
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
#endif
//set timer0 interrupt at 61Hz ( as close to 60Hz as feasible
// because of the requirement to keep OCR0A < 256 )
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 61hz increments
  OCR0A = 255;// = (16*10^6) / (61*1024) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

//set timer2 interrupt at 8kHz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);


  sei();//allow interrupts
  }

void setup()
  {
  unsigned char low;
  unsigned char hi;
  
  digitalWrite( 4, HIGH );

  //start serial connection
  Serial.begin( 9600 );
  //configure pin2 as an input and enable the internal pull-up resistor
  pinMode( 4, OUTPUT );
  
  // configure pin 5 for testing the GRN led mosfet driver
  pinMode( RED_PIN, OUTPUT );
  pinMode( GRN_PIN, OUTPUT );
  pinMode( BLU_PIN, OUTPUT );
  
  digitalWrite( RED_PIN, LOW );
  digitalWrite( GRN_PIN, LOW );
  digitalWrite( BLU_PIN, LOW );

#ifdef  MATRIX 
  matrix.begin(0x70);
#endif

  // start the SPI library:
  SPI.setDataMode( SPI_MODE3 );
  SPI.setBitOrder( MSBFIRST );
  
  SPI.begin();

#ifdef  ENCODER  
  AdaEncoder::addEncoder( 'a', a_PINA, a_PINB );
#endif
#ifdef  MATRIX
  matrix.print( value, HEX );
  matrix.writeDisplay();
#endif

  // setup Analog reference to default of 5V
//  analogReference( DEFAULT );
  analogReference( EXTERNAL );
  
  Serial.println( "Mercury, 0.10.0" );
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
 
  setupTimers(); 
  
  // setup fade routine
  iFade  = 0;
  redPWM = MAX_PWM >> 2;
  grnPWM = MAX_PWM >> 2;
  bluPWM = MAX_PWM >> 2;
  timer0Routine = NULL;
  }
  
ISR( TIMER0_COMPA_vect ){//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
/*  if (toggle0){
    digitalWrite(8,HIGH);
    toggle0 = 0;
  }
  else{
    digitalWrite(8,LOW);
    toggle0 = 1;
  }
*/
  fTimer0 = 1;
}

ISR( TIMER1_COMPA_vect ){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5Hz (takes two cycles for full wave- toggle high then toggle low)
/*
  if (toggle1){
    digitalWrite(13,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(13,LOW);
    toggle1 = 1;
  }
*/  
  fTimer1 = 1;
}
  
ISR( TIMER2_COMPA_vect ){//timer1 interrupt 8kHz toggles pin 9
//generates pulse wave of frequency 8kHz/2 = 4kHz (takes two cycles for full wave- toggle high then toggle low)
/*
  if (toggle2){
    digitalWrite(9,HIGH);
    toggle2 = 0;
  }
  else{
    digitalWrite(9,LOW);
    toggle2 = 1;
  }
*/  
  fTimer2 = 1;
}

void serialEvent()
  {
  while ( Serial.available() )
    {
    char c = (char) Serial.read();
    if ( c == '\n' || c == '\r' )
      {
      c = 0;
      fCmdAvailable = 1;
      }
    szInputBuf[ iInputBuf++ ] = c;
    }
  }
  

void loop()
{
  int ad;

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


#ifdef  ENCODER  
  encoder *thisEncoder;
  thisEncoder=AdaEncoder::genie( &clicks, &id );
  if ( thisEncoder != NULL ) {
    Serial.print( id ); Serial.print(':');
    if ( clicks > 0 ) {
      Serial.println(" CW");
    }
    if ( clicks < 0 ) {
       Serial.println(" CCW");
    }
#endif
#ifdef  MATRIX
  value += clicks;  
  matrix.print( value, HEX );
  matrix.writeDisplay();
  }
#endif

  
  if ( fTimer0 )
    {
    fTimer0 = 0;
    if ( timer0Routine )
      (*timer0Routine)();
    digitalWrite( 4, f4 ? HIGH : LOW );
    f4 = ! f4;
    }
    
  if ( fTimer1 )
    {
#ifdef  MATRIX      
    matrix.print( value++, HEX );
    matrix.writeDisplay();
#endif    
    fTimer1 = 0;
    }

  if ( fTimer2 )
    {
    fTimer2 = 0;
    
    digitalWrite( RED_PIN, ( cycle < redPWM ?  HIGH : LOW ) );
    digitalWrite( GRN_PIN, ( cycle < grnPWM ?  HIGH : LOW ) );
    digitalWrite( BLU_PIN, ( cycle < bluPWM ?  HIGH : LOW ) );
    if ( ++cycle == MAX_PWM )
      cycle = 0;
    }
    
  if ( fCmdAvailable )
    {
    switch ( szInputBuf[ 0 ] )
      {
#ifdef  MATRIX        
        case 'b':
          if ( szInputBuf[ 1 ] == '+' )
            {
            if ( ++brightness > 15 )
              brightness = 15;
            }
          else
            {
            if ( brightness-- == 0 )
              brightness = 0;
            }
//            matrix.setBrightness( brightness );
          break;
#endif

        case 'A':
          redPWM = 0;
          grnPWM = 0;
          bluPWM = 0;
          timer0Routine = policeLights;
          break;
          
        case 'C':
          Serial.println( (char *) szInputBuf );
          break;
          
        case 'D':
          redPWM = MAX_PWM >> 2;
          grnPWM = MAX_PWM >> 2;
          bluPWM = MAX_PWM >> 2;
          timer0Routine = NULL;
          break;

        case 'F':
          redPWM = 0;
          grnPWM = 0;
          bluPWM = 0;
          timer0Routine = continuousFade;
          break;
          
        case 'G':
          grnPWM = MAX_PWM;
          redPWM = MAX_PWM;
          bluPWM = MAX_PWM;
          iGreenToBlack = 0;
          timer0Routine = greenToBlack;
          break;
          
        case 'E':
          grnPWM = MAX_PWM;
          redPWM = MAX_PWM;
          bluPWM = MAX_PWM;
          iRedToBlack = 0;
          timer0Routine = redToBlack;
          break;

        case 'R':
#ifdef  MATRIX        
          value = 0;
#endif          
          redPWM = 0;
          grnPWM = 0;
          bluPWM = 0;
          timer0Routine = NULL;          
          break;
          
        case 'W':
          redPWM = MAX_PWM >> 2;
          grnPWM = 0;
          bluPWM = 0;
          timer0Routine = NULL;
          break;

        default:
          break;
      }
      
    fCmdAvailable = 0;
    iInputBuf  = 0;
    }  
}



