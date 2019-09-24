/*
 * A small snake game for mBot IR remote and LED matrix (connected to port 2).
 */

// IR library setup.
#include <IRLibDecodeBase.h> // First include the decode base.
#include <IRLib_P01_NEC.h> // Now include only the protocols you need.
#include <IRLibCombo.h> // After all protocols, include this.
// All of the above automatically creates a universal decoder
// class called "IRdecode" containing only the protocols you want.
// Now declare an instance of that decoder.
IRdecode decoder;
// Include a receiver, either this or IRLibRecvPCI or IRLibRecvLoop.
#include <IRLibRecv.h> 
IRrecv receiver(2);  // Pin number for the receiver.

// Remote control code constants!
const uint32_t REMOTE_GEAR{ 0xFFA857 };
const uint32_t REMOTE_UP{ 0xFF02FD };
const uint32_t REMOTE_DOWN{ 0xFF9867 };
const uint32_t REMOTE_LEFT{ 0xFFE01F };
const uint32_t REMOTE_RIGHT{ 0xFF906F };
const uint32_t REMOTE_0{ 0xFF6897 };
const uint32_t REMOTE_1{ 0xFF30CF };
const uint32_t REMOTE_2{ 0xFF18E7 };
const uint32_t REMOTE_3{ 0xFF7A85 };
const uint32_t REMOTE_4{ 0xFF10EF };
const uint32_t REMOTE_5{ 0xFF38C7 };
const uint32_t REMOTE_6{ 0xFF5AA5 };
const uint32_t REMOTE_7{ 0xFF42BD };
const uint32_t REMOTE_8{ 0xFF4AB5 };
const uint32_t REMOTE_9{ 0xFF52AD };
const uint32_t REMOTE_A{ 0xFFA25D };
const uint32_t REMOTE_B{ 0xFF629D };
const uint32_t REMOTE_C{ 0xFFE21D };
const uint32_t REMOTE_D{ 0xFF22DD };
const uint32_t REMOTE_E{ 0xFFC23D };
const uint32_t REMOTE_F{ 0xFFB04F };

// LED Matrix library setup.
#include <Adafruit_GFX.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>
TM1640 module(10, 9); // DIN, SCLK. For mBot port 2, DIN=10, SCLK=9..
const int WIDTH{ 16 };
const int HEIGHT{ 8 };
const int NUM_PIXELS{ WIDTH * HEIGHT };
TM16xxMatrixGFX matrix(&module, WIDTH, HEIGHT);
const unsigned char GEAR_BITMAP [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2a, 0x1c, 0x36, 0x1c, 0x2a, 0x00, 0x3a, 0x00
};
const int LIGHT_SENSOR{ A6 };

// Buzzer library setup.
#include <NewTone.h>
const int BUZZER_PIN{ 8 };
const int NOTE_A4{ 440 };
const int NOTE_C5 { 523 };
const int NOTE_CS5 { 554 };
const int NOTE_E5 { 659 };
const int NOTE_A5{ 880 };
const int START_JINGLE_FREQS[]{ NOTE_A4, NOTE_CS5, NOTE_E5, NOTE_A5 };
const int START_JINGLE_DELAYS[]{ 125, 125, 125, 125 };
const int START_JINGLE_LENGTH{ 4 };
const int WIN_JINGLE_FREQS[]{ NOTE_CS5, NOTE_A4, NOTE_E5, NOTE_A5, 0, NOTE_A5, 0, NOTE_A5 };
const int WIN_JINGLE_DELAYS[]{ 125, 125, 125, 125, 175, 175, 225, 225 };
const int WIN_JINGLE_LENGTH{ 8 };
const int LOSE_JINGLE_FREQS[]{ NOTE_A5, NOTE_E5, NOTE_C5, NOTE_A4, 0, NOTE_A4, 0, NOTE_A4 };
const int LOSE_JINGLE_DELAYS[]{ 125, 125, 125, 125, 175, 175, 225, 225 };
const int LOSE_JINGLE_LENGTH{ 8 };

void playJingle(const int frequencies[], const int delays[], uint8_t length)
{
  for (int i = 0; i < length; i++)
  {
    if (frequencies[i] > 0)
      NewTone(BUZZER_PIN, frequencies[i]);
    else
      noNewTone(BUZZER_PIN);
    delay(delays[i]);
  }
  noNewTone(BUZZER_PIN);
}

// Constants and variables for the game data.

// Dump the game data to the serial port for debugging.
void debug(int nextHead=-1)
{
  Serial.print("Snake: ");
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
  {
    Serial.print(snake[i]);
    Serial.print(", ");
  }
  Serial.println("");
  Serial.print("Head: ");
  Serial.print(head);
  if (nextHead >= 0)
  {
    Serial.print("   Next Head: ");
    Serial.print(nextHead);
  }
  Serial.println("");
  Serial.print("Tail: ");
  Serial.println(tail);
  Serial.print("Dir: ");
  Serial.println(dir);
  Serial.print("Apple: ");
  Serial.println(apple);
}

// Variables for timing.



// Arduino setup() function... this is called once at the beginning!
void setup()
{
  Serial.begin(115200);
  receiver.enableIRIn(); // Start the receiver.
  Serial.println("Ready to receive IR signals!");

  // For now... so we know the robot is alive!
  playJingle(START_JINGLE_FREQS, START_JINGLE_DELAYS, START_JINGLE_LENGTH);
}



// Arduino loop() function... this is called again and again forever!
void loop()
{

}
