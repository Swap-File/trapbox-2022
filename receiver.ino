/* Serial laser communicator, receiver

  The circuit:
   laser VCC pin to pin 6, - pin to ground
   photodiode signal on pin 7, power pins as normal

  Note:
   timer 2 used
   For arduino uno or any board with ATMEL 328/168.. diecimila, duemilanove, lilypad, nano, mini...

  Author: Andrew R. from HobbyTransform (http://hobbytransform.com/)
  Written in 2016
*/

#include <HT_hamming_encoder.h>
#include <HT_light_modulator.h>
#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 16 // Popular NeoPixel ring size

// twelve servo objects can be created on most boards
Adafruit_NeoPixel pixels(NUMPIXELS, 2, NEO_GRB + NEO_KHZ800);

HT_PhotoReceiver pdiode;

void setup() {
  digitalWrite(A0, HIGH);
  pinMode(A0, OUTPUT);


  pixels.begin();

  pixels.clear();
  pixels.show();

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(16, 0, 0));
    pixels.show();
    delay(200);
  }

  digitalWrite(A0, LOW);
  delay(200);
  digitalWrite(A0, HIGH);

  pdiode.set_speed(25000);      // bits/sec, must match laser bit transfer rate
  pdiode.begin();

  Serial.begin(9600);


  pixels.clear();
  pixels.show();
}//end setup


//timer2 interrupts LIGHT_RECEIVE_PIN at determined speed to receive each half bit
//via the manchester modulated signal.
ISR(TIMER2_COMPA_vect) {

  //receive message, if any
  pdiode.receive();

}

char laser_buffer[10]; // secret
int buffer_index = 0;
void loop() {

  //print and return most recently received byte, if any, only once
  char temp = pdiode.printByte();
  laser_buffer[4] = '\0';

  if (temp != laser_buffer[3]) {
    laser_buffer[0] = laser_buffer[1];
    laser_buffer[1] = laser_buffer[2];
    laser_buffer[2] = laser_buffer[3];
    laser_buffer[3] =  temp;
  }

  if ( strcmp(laser_buffer, "lock") == 0) {

    digitalWrite(A0, LOW);

    while (1) {
      for (int i = 0; i < NUMPIXELS; i++) {
        float val = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
        val = (val / 255) * 64;
        val += 1;
        pixels.clear();
        pixels.setPixelColor(i, pixels.Color(0, val, 0));
        pixels.setPixelColor((i + 8) % 16, pixels.Color(0, val, 0));
        pixels.show();
        delay(20);
      }
       digitalWrite(A0, HIGH);
    }
  }

}
