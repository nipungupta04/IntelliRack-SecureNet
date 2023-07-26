#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int count = 0;
int card_no[12];
int master[12] = { 51, 53, 48, 48, 52, 65, 55, 49, 65, 70, 65, 49 };
int reset_key[12] = {51, 53, 48, 48, 52, 65, 55, 49, 66, 51, 66, 68};

unsigned long timer, timer1;
int flag = 1;
int off_on = 1;  //0 mean on
#define max_cards 14;
int indexAddress = 0;
int arrayAddress = sizeof(int);
int index;
int card_arr [14][12];
int log_status = 1;

#define shutdown_time 3

#define LED_Green 9  //pin 9
#define buzzer 10    //pin 10
#define relay 6      //pin 6

int disp = 0;

void display_toScreen_mono(const char* message, int x, int y, int delay_time = 0);
void display_toScreen_serif(const char* message, int x, int y, int delay_time = 0);
void log_sequence( String on_close, int override = 0);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  EEPROM.get(indexAddress, index);
  EEPROM.get(arrayAddress, card_arr);
  intial_check();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  pinMode(LED_Green, OUTPUT);
  digitalWrite(LED_Green, LOW);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  display_starting();  //loop and delay inbuilt 5s

  timer1 = millis();
  timer = millis() + 300000;

  const char* msg = "SOLVISON";
  display_toScreen_mono(msg, 20, 22, 0);
}

void loop() {

  const char* msg = "SOLVISON";
  display_toScreen_mono(msg, 20, 22, 0);
  log_sequence("open");


  if (Serial1.available()) {
    disp = 0;
    card_no[count] = Serial1.read();
    count++;
    if (count == 12) {
      flag = 0;
      count = 0;
    }
  }

  if (flag == 0) {

    int code = 1;
    int resetKey_status = 1;
    for (int i = 0; i < 12; i++) {
      if (card_no[i] != reset_key[i])
        resetKey_status = 0;
    }

    if (resetKey_status == 1)
    {
      EEPROM.put(indexAddress, 0);
      index = 0;
      const char* msg = "Erasing Keys";
      display_toScreen_serif(msg, 10, 22, 2);
      flush_Serial1();
      code = 0;
    }

    int master_status = 1;
    for (int i = 0; i < 12; i++) {
      if (card_no[i] != master[i])
        master_status = 0;
    }

    if (master_status == 1) {
      const char* msg = "MASTER KEY";
      display_toScreen_mono(msg, 10, 22, 2);

      msg = "Touch Card";
      display_toScreen_mono(msg, 15, 22, 0);
      int new_card[12];
      int count_new = 0;
      int recorded = 0;
      unsigned long putcard_timer = millis();

      flush_Serial1();
      while (1) {
        if (Serial1.available()) {
          new_card[count_new] = Serial1.read();
          count_new++;
          if (count_new == 12) {
            msg = "Processing";
            display_toScreen_mono(msg, 10, 22, 0);
            flush_Serial1();
            for (int i = 0; i < 12; i++) {

            }

            recorded = 1;
            delay(2000); //for processing display
            count_new = 0;
            break;
          }
        }

        if ((millis() - putcard_timer) > 15000)
        {
          putcard_timer = 0;
          break;
        }
      }

      if (recorded == 1) {
        if (is_dublicate(new_card))
        {
          const char* msg = "Dublicate";
          display_toScreen_serif(msg, 10, 22, 2);
          flush_Serial1();
        }
        else {
          index++;
          for (int i1 = 0; i1 < 12; i1++) {

            card_arr[index][i1] = new_card[i1];
          }

          EEPROM.put(indexAddress, index);
          EEPROM.put(arrayAddress, card_arr);

          const char* msg = "Registered";
          display_toScreen_mono(msg, 15, 22, 2);
          flush_Serial1();
        }
      }
    }


    else if (index == 0 && master_status == 0 && resetKey_status != 1) {
      const char* msg = "Invalid Card";
      display_toScreen_serif(msg, 15, 22, 2);

      delay(3000);
    }

    else if (master_status == 0 && index > 0) {
      for (int k = 1; k <= index; k++) {
        code = 1;
        for (int i = 0; i < 12; i++) {
          if (card_no[i] != card_arr[k][i]) {
            code = 0;
            break;
          }
        }
        if (code == 1)
          break;
      }
      if (code == 0) {
        const char* msg = "Invalid Card";
        display_toScreen_serif(msg, 10, 22, 2);
        code = 0;
        delay(3000);
      }

      else if (code == 1) {
        if (off_on == 0 && millis() - timer > 30000) {
          if (disp == 0) {
            digitalWrite(buzzer, HIGH);
            const char* msg = "Authenticated";
            display_toScreen_serif(msg, 15, 22, 2);
            disp = 1;
          }
          shutdown_sequence();  // has a delay 2s
          delay(3000);
          display_timer();
          digitalWrite(relay, LOW);
          digitalWrite(LED_Green, LOW);
          timer = millis();
          off_on = 1;
        }

        if (off_on == 1 && millis() - timer > 60000) {
          digitalWrite(LED_Green, HIGH);
          digitalWrite(relay, HIGH);
          log_status = 0;
          digitalWrite(buzzer, HIGH);
          off_on = 0;
          timer = millis();
          if (disp == 0) {
            const char* msg = "Authenticated";
            display_toScreen_serif(msg, 15, 22, 2);
            disp = 1;
          }
          delay(2000);
          digitalWrite(buzzer, LOW);
          delay(3000);
        }
      }
    }
    flag = 1;
    count = 0;
  }
}



void intial_check() {
  if (index == 255 || index < 0) {
    index = 0;
    EEPROM.write(0, index);
  }
}

void flush_Serial1()
{
  while (Serial1.available()) {
    Serial1.read();
  }
}

bool is_dublicate(int new_card[12]) {
  for (int i = 0; i < index; i++) {
    bool match = true;  // Flag to track if a row matches
    for (int j = 0; j < 12; j++) {
      if (new_card[j] != card_arr[i][j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return true;  // Match found, return true
    }
  }

  return false;  // No match found
}
