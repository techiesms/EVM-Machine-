
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <AceButton.h>    // https://github.com/bxparks/AceButton (1.9.2)
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool Vote1_done = 0;
bool Vote2_done = 0;
bool Vote3_done = 0;
bool Vote4_done = 0;

// GPIO for switch
static uint8_t switch1 = 32;
static uint8_t switch2 = 35;
static uint8_t switch3 = 34;
static uint8_t switch4 = 39;
static uint8_t switch5 = 33;
static uint8_t switch6 = 27;
static uint8_t switch7 = 14;
static uint8_t switch8 = 12;


int Buzzer = 4;

int LED1 = 25;
int LED2 = 26;
int LED3 = 16;
int LED4 = 17;

using namespace ace_button;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);
ButtonConfig config5;
AceButton button5(&config5);
ButtonConfig config6;
AceButton button6(&config6);
ButtonConfig config7;
AceButton button7(&config7);
ButtonConfig config8;
AceButton button8(&config8);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);
void handleEvent5(AceButton*, uint8_t, uint8_t);
void handleEvent6(AceButton*, uint8_t, uint8_t);
void handleEvent7(AceButton*, uint8_t, uint8_t);
void handleEvent8(AceButton*, uint8_t, uint8_t);

String dataMessage;

// Initialize SD card
void initSDCard()
{
  Serial.println("Setting Up SD Card");

  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 10);            // Start at top-left corner
  display.println("Setting up ");
  display.println(" SD Card ");
  display.display();

  delay(2000);

  if (!SD.begin())
  {
    Serial.println("Card Mount Failed");

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 10);            // Start at top-left corner
    display.println("Card Mount ");
    display.println(" Failed ");
    display.display();
    beep(5000);
    delay(5000);
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("     No ");
    display.println("  SD Card ");
    display.display();
    beep(5000);
    delay(5000);
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 10);            // Start at top-left corner
    display.println("  Card ");
    display.println(" Detected ");
    display.display();

    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(50);
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(50);
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    delay(50);

    delay(2000);
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

}

// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("  Data ");
    display.println("  Stored ");
    display.display();

    delay(1000);
  } else {
    Serial.println("Write failed");

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("  No Data ");
    display.println("  Stored ");
    display.display();
    beep(5000);
    delay(5000);
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    Serial.println("Append failed");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("No Data ");
    display.println("  Stored ");
    display.display();
    beep(5000);
    delay(5000);
    return;
  }
  if (file.print(message))
  {
    Serial.println("Message appended");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("  Data ");
    display.println("  Stored ");
    display.display();
    delay(1000);

  } else
  {
    Serial.println("Append failed");
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("No Data ");
    display.println("  Stored ");
    display.display();
    beep(5000);
    delay(5000);
  }
  file.close();
}

void setup()
{
  Serial.begin(115200);

  // Configure the input GPIOs
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);
  pinMode(switch5, INPUT_PULLUP);
  pinMode(switch6, INPUT_PULLUP);
  pinMode(switch7, INPUT_PULLUP);
  pinMode(switch8, INPUT_PULLUP);

  pinMode(Buzzer, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  // display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 10);            // Start at top-left corner
  display.println("Setting up Machine");
  display.display();
  Serial.println("Setting Up Machine");

  initSDCard();

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if (!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", "HeadBoy Candidate1,HeadBoy Candidate2,HeadGirl Candidate1,HeadGirl Candidate2,Captain Candidate1, Captain Candidate2, Vice Captain Candidate1, Vice Captain Candidate2  \r\n");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);
  config5.setEventHandler(button5Handler);
  config6.setEventHandler(button6Handler);
  config7.setEventHandler(button7Handler);
  config8.setEventHandler(button8Handler);

  button1.init(switch1);
  button2.init(switch2);
  button3.init(switch3);
  button4.init(switch4);
  button5.init(switch5);
  button6.init(switch6);
  button7.init(switch7);
  button8.init(switch8);
}

void loop()
{


  if (Vote1_done == 0 && Vote2_done == 0 && Vote3_done == 0 && Vote4_done == 0)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 15);            // Start at top-left corner
    display.println("   Head ");
    display.println("    Boy ");
    display.display();



    button1.check();
    button2.check();
  }
  if (Vote1_done == 1 && Vote2_done == 0 && Vote3_done == 0 && Vote4_done == 0)
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 15);            // Start at top-left corner
    display.println("   Head ");
    display.println("    Girl ");
    display.display();



    button3.check();
    button4.check();
  }
  if (Vote1_done == 1 && Vote2_done == 1 && Vote3_done == 0 && Vote4_done == 0)
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(25, 25);            // Start at top-left corner
    display.println("Captain ");
    display.display();


    button5.check();
    button6.check();
  }
  if (Vote1_done == 1 && Vote2_done == 1 && Vote3_done == 1 && Vote4_done == 0)
  {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);

    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);            // Start at top-left corner
    display.println("   Vice ");
    display.println("  Captain ");
    display.display();



    button7.check();
    button8.check();
  }
  if (Vote1_done == 1 && Vote2_done == 1 && Vote3_done == 1 && Vote4_done == 1)
  {
    // Clear the buffer
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 15);            // Start at top-left corner
    display.println("Thanks for");
    display.println(" your Vote");
    display.display();

    Serial.println("Thanks for your Vote");
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    Vote1_done = 0;
    Vote2_done = 0;
    Vote3_done = 0;
    Vote4_done = 0;

    beep(2000);
    delay(5000);
  }

}


void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState)
{
  switch (eventType)
  {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(20, 15);            // Start at top-left corner
      display.println("Head Boy ");
      display.println("Candidate1");
      display.display();
      Serial.println("Voted for Head Boy Candidate1");
      dataMessage =  "1,0,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);
      //Append the data to file
      Vote1();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());

      delay(1500);

      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {

  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(20, 15);            // Start at top-left corner
      display.println("Head Boy ");
      display.println("Candidate2");
      display.display();
      Serial.println("Voted for Head Boy Candidate2");
      dataMessage =  "0,1,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote1();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      delay(1500);
      break;


  }

}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 3");

  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(5, 10);            // Start at top-left corner
      display.println("Head Girl ");
      display.println("Candidate1");
      display.display();
      Serial.println("Voted for Head Girl Candidate1");
      dataMessage =  "1,0,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote2();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 4");

  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(5, 10);            // Start at top-left corner
      display.println("Head Girl ");
      display.println("Candidate2");
      display.display();
      Serial.println("Voted for Head Girl Candidate2");
      dataMessage =  "0,1,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);


      //Append the data to file
      Vote2();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}

void button5Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 5");
  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0, 20);            // Start at top-left corner
      display.println(" Captain ");
      display.println("Candidate1");
      display.display();
      Serial.println("Voted for Captain Candidate1");
      dataMessage =  "1,0,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote3();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}

void button6Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 6");
  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0, 10);            // Start at top-left corner
      display.println("  Captain ");
      display.println("Candidate2");
      display.display();
      Serial.println("Voted for Captain Candidate2");
      dataMessage =  "0,1,";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote3();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}


void button7Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 7");
  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0, 0);            // Start at top-left corner
      display.println("   Vice");
      display.println(" Captain");
      display.println("Candidate1");
      display.display();
      Serial.println("Voted for Vice Captain Candidate1");
      dataMessage =  "1,0 \r\n";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote4();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}

void button8Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Vote for Candidate 8");
  switch (eventType) {
    case AceButton::kEventPressed:
      // Clear the buffer
      display.clearDisplay();
      display.setTextSize(2);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0, 0);            // Start at top-left corner
      display.println("   Vice");
      display.println(" Captain");
      display.println("Candidate2");
      display.display();
      Serial.println("Voted for Vice Captain Candidate2");
      dataMessage =  "0,1 \r\n";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      Vote4();
      delay(500);
      appendFile(SD, "/data.txt", dataMessage.c_str());
      //pref.putBool("Relay1", switch_state_ch1);

      break;
  }
}


void Vote1()
{

  beep(100);
  Vote1_done = 1;

}

void Vote2()
{

  beep(100);
  Vote2_done = 1;

}

void Vote3()
{

  beep(100);
  Vote3_done = 1;

}

void Vote4()
{

  beep(100);
  Vote4_done = 1;

}



void beep(int delay_period)
{
  digitalWrite(Buzzer, HIGH);
  delay(delay_period);
  digitalWrite(Buzzer, LOW);
  delay(500);
}
