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
const int MAX_SNAKE_LENGTH{20}; // When the snake gets this long, we win!
int snake[MAX_SNAKE_LENGTH]{}; // The position of each segment of the snake's body.
int head{}; // Array index of the snake's head.
int tail{}; // Array index of the snake's tail.
// To move, add one of these constants to the current position.
const int DIR_UP{ -WIDTH };
const int DIR_DOWN{ WIDTH };
const int DIR_LEFT{ -1 };
const int DIR_RIGHT{ 1 };
int dir{ DIR_RIGHT }; // The current direction of movement.
int apple{}; // The position of the apple.

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
unsigned long nextTurn{}; // In milliseconds.
unsigned long turnTime{250}; // In milliseconds.
bool gameRunning{false};

// Set the brightness of the display based on the light sensor.
void setIntensity()
{
  uint16_t lightness{ analogRead(LIGHT_SENSOR) / 100 };
  lightness = (lightness < 1) ? 1 : ((lightness > 7) ? 7 : lightness); // Clamp.
  matrix.setIntensity(lightness); // Use a value between 0 and 7 for brightness.
}

// Turn on or off one pixel on the display.
void setPixel(int address, bool value)
{
  // set one pixel in the memory bitmap on.
  matrix.drawPixel(address / WIDTH, address % WIDTH, value);
}

// Draw the start image on the display.
void drawStart()
{
  setIntensity();
  matrix.fillScreen(LOW); // Clear the matrix.
  matrix.drawBitmap(0, 0, GEAR_BITMAP, 8, 16, 1);
  matrix.write();
}

// Set the game variables to start a new game.
void newGame()
{
  // Clear the snake array.
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
    snake[i] = -1;
  tail = 0;
  snake[0] = 2 + 4 * WIDTH; // Column 2, Row 4.
  snake[1] = snake[0] + 1; // Column 3.
  snake[2] = snake[0] + 2; // Column 4.
  head = 2;
  dir = DIR_RIGHT;
  apple = 10 + 4 * WIDTH; // Column 12, Row 4.

  // Clear the display and draw the current position.
  setIntensity();
  matrix.fillScreen(LOW); // Clear the matrix.
  setPixel(apple, HIGH);
  // Draw the snake...
  for (int i = tail; i <= head; i++)
    setPixel(snake[i], HIGH);
  matrix.write(); // Send the memory bitmap to the display.

  playJingle(START_JINGLE_FREQS, START_JINGLE_DELAYS, START_JINGLE_LENGTH);

  // Schedule the next turn and start the game!
  nextTurn = millis() + turnTime;
  gameRunning = true;
}

// Arduino setup() function... this is called once at the beginning!
void setup()
{
  Serial.begin(115200);
  receiver.enableIRIn(); // Start the receiver.
  Serial.println("Ready to receive IR signals!");
}

// Get a button press from the remote and decide what to do.
void handleRemote()
{
  if (receiver.getResults()) { 
    decoder.decode();
    // Use this to research the remote protocol/button values.
    // Use true for more detail, false for less.
    // decoder.dumpResults(true);
    // Handle the remote buttons.
    // Note: We're not allowed to do a 180 turn!
    switch (decoder.value) {
      case REMOTE_UP:
        if (dir != DIR_DOWN)
          dir = DIR_UP;
        break;
      case REMOTE_DOWN:
        if (dir != DIR_UP)
          dir = DIR_DOWN;
        break;
      case REMOTE_LEFT:
        if (dir != DIR_RIGHT)
          dir = DIR_LEFT;
        break;
      case REMOTE_RIGHT:
        if (dir != DIR_LEFT)
          dir = DIR_RIGHT;
        break;
      case REMOTE_GEAR: // Start the game if it isn't running.
        if (!gameRunning)
          newGame();
        Serial.println("Gear button!");
        break;
      case REMOTE_C: // Give us a way to stop the game, for testing.
        gameRunning = false;
        Serial.println("Button C!");
        break;
      case REMOTE_7: // Game over, for testing.
        gameOver();
        Serial.println("Button 7!");
        break;
      case REMOTE_8: // You win, for testing.
        youWin();
        Serial.println("Button 8!");
        break;
      case REPEAT_CODE: // Ignore repeats!
        break;
      default:
        Serial.println("Unknown button...");
    }
    receiver.enableIRIn(); // Don't forget to restart receiver!
  }
}

// Do stuff when the player loses...
void gameOver()
{
  gameRunning = false;
  playJingle(LOSE_JINGLE_FREQS, LOSE_JINGLE_DELAYS, LOSE_JINGLE_LENGTH);
  delay(500);
}

// Do stuff when the player wins...
void youWin()
{
  gameRunning = false;
  playJingle(WIN_JINGLE_FREQS, WIN_JINGLE_DELAYS, WIN_JINGLE_LENGTH);
  delay(500);
}

// Check if the given pixel is in the snake.
bool inSnake(int pixel)
{
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++)
    if (pixel == snake[i])
      return true;
  return false;
}

// Update the game for one move.
void takeTurn()
{
  int lastHeadPixel{ snake[head] }; // The head pixel from last time.
  head = (head + 1) % MAX_SNAKE_LENGTH; // Get the next head index.
  int nextHead{ lastHeadPixel + dir }; // Move the head one pixel.
  // debug(nextHead);
  // Check if we ate an apple.
  if (nextHead == apple)
  {
    // Check if we won, i.e. the snake filled the array!
    if ((head + 1) % MAX_SNAKE_LENGTH == tail)
    {
      youWin();
      return;
    }
    else
    {
      // Make a new apple. Don't change the tail.
      // The apple pixel becomes the new head, so no change.
      while (inSnake(apple = random(NUM_PIXELS)));
      setPixel(apple, HIGH);
      matrix.write();
    }
  }
  else
  {
    // This is a normal turn.
    setPixel(nextHead, HIGH); // Turn on the pixel for the new head.
    if (snake[tail] != nextHead) // This is allowed because it's the old tail.
      setPixel(snake[tail], LOW); // Turn off the old tail.
    snake[tail] = -1; // Clear that position in the array.
    tail = (tail + 1) % MAX_SNAKE_LENGTH; // Get the next tail index.
  }
  // Check for a crash against an edge or with itself.
  if (dir == DIR_RIGHT && nextHead % WIDTH == 0
   || dir == DIR_LEFT && nextHead % WIDTH == WIDTH - 1
   || nextHead >= NUM_PIXELS || nextHead < 0
   || inSnake(nextHead))
  {
    gameOver();
    return;
  }
  snake[head] = nextHead; // Wait until after the crash check!
  setIntensity();
  matrix.write(); // Draw the changes.
}

// Arduino loop() function... this is called again and again forever!
void loop()
{
  handleRemote();
  if (gameRunning)
  {
    if (millis() > nextTurn)
    {
      // TODO: Use the analog sensor to control the speed!
      nextTurn += turnTime; // Schedule the next turn.
      takeTurn();
    }
  }
  else
  {
    // If the game isn't running, keep redrawing the start image!
    drawStart();
  }
}
