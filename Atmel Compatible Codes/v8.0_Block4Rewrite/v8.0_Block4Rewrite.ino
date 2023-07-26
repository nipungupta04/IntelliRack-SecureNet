#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
SoftwareSerial SWSerial(14, 15);
PN532_SWHSU pn532swhsu(SWSerial);
PN532 nfc(pn532swhsu);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint8_t KeyMaker[4] = {115, 127, 54, 67};
uint8_t reset_key[4] = {147, 165, 113, 66};
uint8_t Master[4] = {0, 0, 0, 0};
uint8_t Authentication_bits[4] {4, 3, 20, 2};
bool Authenticated_card = true;

uint8_t uid_Auth[] = { 0, 0, 0, 0 };
uint8_t uidLength;
uint8_t data[16];
bool recieved_confirmation = false;

unsigned long timer, timer1;
int flag = 1;
int off_on = 1;  //0 mean on
const int max_cards = 6; // will actually work as 5 idk why
int indexAddress = 0;
int arrayAddress = sizeof(int);
int index;
uint8_t card_arr [max_cards][4];
int log_status = 1;

#define shutdown_time 3

#define LED_Green 9  //pin 9
#define buzzer 10    //pin 10
#define relay 6      //pin 6

int disp = 0;

void display_toScreen(const char* message, int x, int y, int delay_time = 0, int text_size = 2);
void log_sequence( String on_close, int override = 0);
void display_starting();
void intial_check();
void flush_Serial1();
void readNFC();
bool is_Master(uint8_t new_card[]);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  EEPROM.get(indexAddress, index);
  EEPROM.get(arrayAddress, card_arr);
  intial_check();
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata)
    while (1);
  nfc.SAMConfig();
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  pinMode(LED_Green, OUTPUT);
  digitalWrite(LED_Green, LOW);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  display_starting();
  timer1 = millis();
  timer = millis() + 300000;
}

void loop() {

  const char* msg = "SOLVISION";
  display_toScreen(msg, 15, 10, 0);
  log_sequence("open");
  uidLength = 0;
  readNFC();

  if (flag == 0) {
    int code = 1;
    int resetKey_status = 1;
    for (int i = 0; i < 4; i++) {
      if (uid_Auth[i] != reset_key[i])
        resetKey_status = 0;
    }

    if (resetKey_status == 1)
    {
      EEPROM.put(indexAddress, 0);
      index = 0;
      const char* msg = "Erasing Keys";
      display_toScreen(msg, 10, 20, 2, 1);
      flush_Serial1();
      code = 0;
    }

    int KeyMaker_status = 1;
    for (int i = 0; i < 4; i++) {
      if (uid_Auth[i] != KeyMaker[i])
        KeyMaker_status = 0;
    }

    if (is_Master(uid_Auth)) {
      const char* msg = "Master Key";
      display_toScreen(msg, 5, 10, 2);
      flush_Serial1();
      code = 1;
      disp = 0;
      goto OverRide_Master;
    }

    if (KeyMaker_status == 1) {
      const char* msg = "Key Maker";
      display_toScreen(msg, 7, 10, 2);
      msg = "Touch Card";
      display_toScreen(msg, 5, 10, 0);
      int recorded = 0;
      unsigned long putcard_timer = millis();
      code = 0;
      flush_Serial1();
      while (1) {
        uidLength = 0;
        readNFC();
        if (uidLength == 4 && Authenticated_card) {
          msg = "Processing";
          display_toScreen(msg, 3, 10, 0);
          flush_Serial1();
          recorded = 1;
          delay(2000);
          uidLength = 0;
          break;
        }

        if ((millis() - putcard_timer) > 15000)
        {
          putcard_timer = 0;
          break;
        }
      }
      if (recorded == 1 && index == max_cards - 1)
      {
        msg = "Limit Over";
        display_toScreen(msg, 4, 10, 0);
        flush_Serial1();
        delay(2000);
      }
      else if (recorded == 1) {
        if (is_Master(uid_Auth))
        {
          const char* msg = "Master Key";
          display_toScreen(msg, 5, 10, 2);
          flush_Serial1();
        }
        if (is_dublicate(uid_Auth) && !is_KeyMaker(uid_Auth))
        {
          const char* msg = "Dublicate";
          display_toScreen(msg, 5, 10, 2);
          flush_Serial1();
        }
        else {
          index++;
          for (int i1 = 0; i1 < 4; i1++) {
            card_arr[index][i1] = uid_Auth[i1];
          }

          EEPROM.put(indexAddress, index);
          EEPROM.put(arrayAddress, card_arr);
          const char* msg = "Registered";
          display_toScreen(msg, 3, 10, 2);
          flush_Serial1();
        }
      }
    }

    else if (index == 0 && KeyMaker_status == 0 && resetKey_status != 1) {
      const char* msg = "Unregistered";
      display_toScreen(msg, 15, 20, 2, 1);
      delay(2000);
    }

    else if (KeyMaker_status == 0 && index > 0) {
      for (int k = 1; k <= index; k++) {
        code = 1;
        for (int i = 0; i < 4; i++) {
          if (uid_Auth[i] != card_arr[k][i]) {
            code = 0;
            break;
          }
        }

        if (code == 1)
          break;
      }

OverRide_Master:
      if (code == 0) {
        const char* msg = "Unregistered";
        display_toScreen(msg, 15, 20, 2, 1);
        code = 0;
        delay(3000);
      }

      else if (code == 1) {
        if (off_on == 0 && millis() - timer > 30000) {
          if (disp == 0) {
            digitalWrite(buzzer, HIGH);
            //Serial.println("on state authenticated");
            const char* msg = "Authenticated";
            display_toScreen(msg, 15, 20, 2, 1);
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
          //Serial.println("off state authenticated");
          digitalWrite(LED_Green, HIGH);
          digitalWrite(relay, HIGH);
          log_status = 0;
          digitalWrite(buzzer, HIGH);
          off_on = 0;
          timer = millis();
          if (disp == 0) {
            const char* msg = "Authenticated";
            display_toScreen(msg, 15, 20, 2, 1);
            disp = 1;
          }
          delay(500);
          digitalWrite(buzzer, LOW);
          delay(3000);
        }
      }
    }
    flag = 1;
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
  while (SWSerial.available()) {
    SWSerial.read();
  }
}

bool is_dublicate(uint8_t new_card[]) {
  for (int i = 1; i <= index; i++) {
    bool match = true;  // Flag to track if a row matches
    for (int j = 0; j < 4; j++) {
      if (new_card[j] != card_arr[i][j]) {
        match = false;
        break;
      }
      if (match && j == 3) {
        return true;  // Match found, return true
      }
    }
  }
  return false;  // No match found
}

bool is_KeyMaker(uint8_t new_card[]) {

  bool yes_no = true;
  for (int j = 0; j < 4; j++) {
    if (new_card[j] != KeyMaker[j]) {
      yes_no = false;
      break;
    }
  }
  return yes_no;
}

bool is_Master(uint8_t new_card[]) {

  bool yes_no = true;
  for (int j = 0; j < 4; j++) {
    if (new_card[j] != Master[j]) {
      yes_no = false;
      break;
    }
  }
  return yes_no;
}
