/* 
    Christmas Tree Music Box:
    Arduino code to make a servo motor spin, LEDs light up, & a buzzer play Jingle Bells simultaneously
    Version: December 6, 2020
*/

// INCLUDES
#include <Servo.h> // include the Servo library

// PIN NUMBERS
const int servoPin = 13;
const int led1Pin = 12;
const int buttonPin = 11;
const int buzzerPin = 10;

// VARIABLES
int startProgram = false;
int doneProgram = false;
int debugMode = false; //NOTE: program runs extremely slow in debug mode due to so many print statements

// LED FLASHING
class Flasher
{
  // Class Member Variables
  int ledPin;      // the number of the LED pin
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time
 
  // These maintain the current state
  int ledState;                 // ledState used to set the LED
  unsigned long previousMillis;   // will store last time LED was updated
 
  // Constructor - creates a Flasher 
  // and initializes the member variables and state
  public:
  Flasher(int pin, long on, long off)
  {
  ledPin = pin;
  pinMode(ledPin, OUTPUT);     
    
  OnTime = on;
  OffTime = off;
  
  ledState = LOW; 
  previousMillis = 0;
  }
 
  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();
     
    if((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;  // Remember the time
      digitalWrite(ledPin, ledState);  // Update the actual LED
    }
    else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      ledState = HIGH;  // turn it on
      previousMillis = currentMillis;   // Remember the time
      digitalWrite(ledPin, ledState);   // Update the actual LED
    }
    
    // print statements
    if(debugMode){
      Serial.println("Flashing LED");
      Serial.println(" ");
    }
  }
  
  void Off()
  {
    ledState = LOW;
  }
}; 

// SERVO MOTOR
class Sweeper
{
  Servo servo;              // the servo
  int pos;              // current servo position 
  int increment;        // increment to move for each interval
  int  updateInterval;      // interval between updates
  unsigned long lastUpdate; // last update of position
 
public: 
  Sweeper(int interval)
  {
    updateInterval = interval;
    increment = 1;
  }
  
  void Attach(int pin)
  {
    servo.attach(pin);
  }
  
  void Detach()
  {
    servo.detach();
  }
  
  void Update()
  {
    if((millis() - lastUpdate) > updateInterval)  // time to update
    {
      lastUpdate = millis();
      pos += increment;
      servo.write(pos);

      // Print Statements
      if(debugMode){
        Serial.println("Position of Servo Motor");
        Serial.println(pos);
        Serial.println(" ");
      }
      
      if ((pos >= 180) || (pos <= 0)) // end of sweep
      {
        // reverse direction
        increment = -increment;
      }
    }
  }
}; 

// MUSIC
const int songLength = 62;
const char notes[songLength] = "eee eee egcde fff ffee eeggfdc eee eee egcde fff ffee eeggfdc" ; // a space represents a rest
const int  beats[songLength] = {2,2,3,1,2,2,3,1,2,2,3,1,4,4,2,2,3,0,1,2,2,2,0,1,1,2,2,2,2,4,4,2,2,3,1,2,2,3,1,2,2,3,1,4,4,2,2,3,0,1,2,2,2,0,1,1,2,2,2,2,4,4};

// testing
//const int songLength = 2;
//const char notes[songLength] = "ee" ; // a space represents a rest
//const int  beats[songLength] = {2,2};

const int tempo = 140;

#define GET_NOTE        0
#define PLAY_NOTE       1
#define SONG_PAUSE      2

int frequency( char note ) 
{
    switch( note )
    {
        case    'c': return 262; break;
        case    'd': return 294; break;
        case    'e': return 330; break;
        case    'f': return 349; break;
        case    'g': return 392; break;
        case    'a': return 440; break;
        case    'b': return 494; break;
        case    'C': return 523; break;
        default: return 0; break;
        
    }
    
}

void PlaySong( void )
{
    static byte
        statePlay = GET_NOTE;
    static int
        noteNumber = 0;
    static unsigned long
        noteDuration = 0,
        noteTime = 0;
    unsigned long
        timeNow;

    switch( statePlay )
    {
        case    GET_NOTE:
            // Print statement
            if(debugMode){
              Serial.println("In GET_NOTE Case");
              Serial.println(" ");
            }
            
            if( notes[noteNumber] != ' ' )
            {
                tone( buzzerPin, frequency( notes[noteNumber] ) );
                
            }
            
            noteDuration = beats[noteNumber] * tempo;  // length of note/rest in ms
            noteTime = millis();
            statePlay = PLAY_NOTE;
            
        break;
        case    PLAY_NOTE:
            // Print statement
            if(debugMode){
              Serial.println("In PLAY_NOTE Case");
              Serial.println(" ");
            }
            if( millis() - noteTime < noteDuration )
                return;
                
            tone( buzzerPin, 0 );
            if( noteNumber < songLength )
            {
                noteNumber++;
                statePlay = GET_NOTE;
                
            }//if
            else
            {
                // Print statement
                Serial.println("STOP_RUNNING");
                Serial.println(" ");
                startProgram = false;
                doneProgram = true;
                noTone(buzzerPin);
                noteNumber = 0;
                noteTime = millis();
                statePlay = GET_NOTE;
                
            }//else                
            
        break;
        
    }
    
}

// MAIN CODE
Sweeper sweeper1(20);
Flasher led1(led1Pin, 123, 400);

// SETUP (at beginning)
void setup() 
{ 
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);  
  pinMode(buttonPin, INPUT);
} 
 
// LOOP (constantly looping)
void loop() 
{ 
  int buttonState = digitalRead(buttonPin); 
  
  // Print statement
  if(debugMode){
    Serial.println("BUTTON STATE");
    Serial.println(buttonState);
    Serial.println(" ");
  }

  // Start program when button's pressed
  if (buttonState) {
    startProgram = true;
  }

  
  if(startProgram) { // when program is running
    sweeper1.Attach(servoPin); 
    sweeper1.Update();
    led1.Update(); 
    PlaySong();
  }
  else if(doneProgram) { // when program is done
    if(debugMode){
      Serial.println("BUTTON STATE");
      Serial.println(buttonState);
      Serial.println(" ");
    }
    sweeper1.Detach(); 
    led1.Off(); 
  }

} 