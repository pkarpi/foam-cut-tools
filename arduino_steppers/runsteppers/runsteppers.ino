#include <RingBuf.h>
// The following is a simple stepper motor control procedures
# define EN 8 // stepper motor enable , active low
# define X_DIR 5 // X -axis stepper motor direction control
# define Y_DIR 6 // y -axis stepper motor direction control
# define Z_DIR 7 // z axis stepper motor direction control
# define A_DIR 12 // A axis stepper motor direction control

# define X_STP 2 // x -axis stepper control
# define Y_STP 3 // y -axis stepper control
# define Z_STP 4 // z -axis stepper control
# define A_STP 13 // A -axis stepper control

unsigned long currentMicros = 0;
unsigned long stepMicros;
unsigned long microsReceive;

long stepPause = 5000;
#define buffer_size 1000

RingBuf<byte, buffer_size> myRingBuffer;

byte byte_array[2];
byte bitti = 0;
byte cmd = 0;
byte xyz, a;

void setup()
{
  pinMode(X_STP, OUTPUT);      // sets the digital pin as output
  pinMode(Y_STP, OUTPUT);      // sets the digital pin as output
  pinMode(Z_STP, OUTPUT);      // sets the digital pin as output
  pinMode(A_STP, OUTPUT);      // sets the digital pin as output
  pinMode(X_DIR, OUTPUT);      // sets the digital pin as output
  pinMode(Y_DIR, OUTPUT);      // sets the digital pin as output
  pinMode(Z_DIR, OUTPUT);      // sets the digital pin as output
  pinMode(A_DIR, OUTPUT);      // sets the digital pin as output

  pinMode(EN, OUTPUT);
  digitalWrite(EN, HIGH);

  digitalWrite(X_DIR, LOW);
  digitalWrite(Y_DIR, LOW);
  digitalWrite(Z_DIR, LOW);
  digitalWrite(A_DIR, LOW);

  digitalWrite(X_STP, LOW);
  digitalWrite(Y_STP, LOW);
  digitalWrite(Z_STP, LOW);
  digitalWrite(A_STP, LOW);
  establishContact();
}

void establishContact()
{
  Serial.begin(115200);     // opens serial port, sets data rate to 9600 bps
  while (!Serial)
  {
  }
}

void loop()
{
  currentMicros = micros();
  if (!myRingBuffer.isEmpty())
  {
    if (myRingBuffer.pop(bitti))
    {
      portStep(bitti, stepPause - (micros() - currentMicros));
      if (bitti == 0) // if end, release steppers
      {
        digitalWrite(EN, HIGH);
      }
    }
  }
  else
  {
    receiveBlock(stepPause - (micros() - currentMicros));
  }
}

void portStep(byte askel, int delayLeft)
{
  stepMicros = micros();

  //First directions
  bitWrite(xyz, 5, bitRead(askel, 2)); //Stepperi 1
  bitWrite(xyz, 6, bitRead(askel, 0)); //Stepperi 2
  bitWrite(xyz, 7, bitRead(askel, 4)); //Stepperi 3
  bitWrite(a, 5, bitRead(askel, 6)); //Stepperi 4

  PORTD = xyz;
  PORTB = a;

  //delayMicroseconds(5);

  //Stepperi 1
  // Luetaan X-stepit oikealta tokat, kirjoitetaan PORTD:n ekaan paikkaan
  bitWrite(xyz, 2, bitRead(askel, 3));
  //Luetaan Y-stepit oikealta ekat, kirjoitetaan tokaan paikkaan
  bitWrite(xyz, 3, bitRead(askel, 1));
  //Stepperi 2
  // Luetaan X-stepit oikealta kolmas, kirjoitetaan kolmanteen paikkaan
  bitWrite(xyz, 4, bitRead(askel, 5));
  // Luetaan Y-stepit oikealta vikat, kirjoitetaan PORTB:n viimeiseen
  bitWrite(a, 4, bitRead(askel, 7));

  PORTD = xyz;
  PORTB = a;

  //delayMicroseconds(5);

  //Bitit alas
  bitWrite(xyz, 2, 0);
  bitWrite(xyz, 3, 0);
  bitWrite(xyz, 4, 0);
  bitWrite(xyz, 5, 0);
  bitWrite(xyz, 6, 0);
  bitWrite(xyz, 7, 0);
  bitWrite(a, 4, 0);
  bitWrite(a, 5, 0);

  PORTD = xyz;
  PORTB = a;

  //delayMicroseconds(5);

  receiveBlock(delayLeft - (micros() - stepMicros));
}

void receiveBlock(int delay)
{
  Serial.write('A');
  microsReceive = micros();
  while ((micros() - microsReceive) < delay)
  {
    if (Serial.available() > 0)
    {
      if (!myRingBuffer.isFull())
      {
        int rlen = Serial.readBytes(byte_array, 2);
        cmd = byte_array[0];
        bitti = byte_array[1];

        if (cmd == 77) //move
        {
          myRingBuffer.push(bitti);
        }
        else if (cmd == 83) //set speed at start
        {
          stepPause = (long)bitti * 100; // convert back to real microseconds value
          digitalWrite(EN, LOW);
        }
        else if (cmd == 69) // end of file
        {
          bitti = 0;
          myRingBuffer.push(bitti);
        }
        else if (cmd == 67) // stop connection
        {
          establishContact();
        }
      }
      else
      {
        Serial.write('K');
      }
    }
  }
  Serial.write('K');
}