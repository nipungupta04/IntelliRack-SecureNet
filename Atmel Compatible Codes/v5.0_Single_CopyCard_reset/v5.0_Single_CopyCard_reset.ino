#include <SPI.h>
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

const int MAX_INDEX = 10;
const int ARRAY_LENGTH = 12;
int indexAddress = 0;
int arrayAddress = sizeof(int);
int index;
int card_arr [MAX_INDEX][ARRAY_LENGTH];


#define shutdown_time 3

#define LED_Green 9  //pin 9
#define buzzer 10    //pin 10
#define relay 6      //pin 6

int disp = 0;

//Serial for computer
//Serial1 for pins Rx Tx
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
  Serial.println("Begin...");
  display_solvision();
  Serial.print("index at start: ");
  Serial.println(index);
  for (int k = 0; k < index; k++) {
    for (int i = 0; i < 12; i++) {
      Serial.print(card_arr[k][i]);
    }
    Serial.println();
  }

}

void loop() {

  display_solvision();
  //Serial.println(millis());
  /*if (off_on > 0)
    Serial.println("off");
    else
    Serial.println("on");

    Serial.print(timer);
    Serial.print(" ");
    Serial.println(millis());*/
  /* if (timer1 - millis() > 30000)
    {
     count = 0;
     timer1 = millis();
     //Serial.println("TIMER1 RESET");
    }*/
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
    /*for (int i = 0; i < 12; i++)
      {
      Serial.print(card_no[i]);
      }*/
    //Serial.println();
    //delay(3000);
    int code = 1;
    int resetKey_status = 1;
    for (int i = 0; i < 12; i++) {
      if (card_no[i] != reset_key[i])
        resetKey_status = 0;
    }
    if (resetKey_status == 1)
    {
      EEPROM.put(indexAddress, 0);
      index=0;
      display_resetDone();
      flush_Serial1();
      code=0;      
    }

    
    int master_status = 1;
    for (int i = 0; i < 12; i++) {
      if (card_no[i] != master[i])
        master_status = 0;
    }

    if (master_status == 1) {
      Serial.println("Master");
      display_master();
      int new_card[12];
      int count_new = 0;
      int recorded = 0;
      unsigned long putcard_timer = millis();
      Serial.print("putcard_timer :");
      Serial.println(putcard_timer);
      flush_Serial1();
      while (1) {
        if (Serial1.available()) {
          new_card[count_new] = Serial1.read();
          Serial.println("reading.. new card");
          count_new++;
          if (count_new == 12) {
            display_processing();
            flush_Serial1();
            for (int i = 0; i < 12; i++) {
              Serial.print("cardno");
              Serial.print(new_card[i]);
            }
            //delay(2000);
            recorded = 1;
            delay(2000); //for processing display
            count_new = 0;
            break;
          }
        }
        Serial.println(millis());
        if ((millis() - putcard_timer) > 15000)
        {
          putcard_timer = 0;
          break;
        }
      }
      Serial.println("Recorded :");
      Serial.println(recorded);
      if (recorded == 1) {
        Serial.println("Entered recorded loop");
        if (is_dublicate(new_card))
        {
          display_dublicate();
          Serial.print("dublicate :");
        }
        else {
          index++;
          for (int i1 = 0; i1 < 12; i1++) {
            Serial.println("Data dumping");
            card_arr[index][i1] = new_card[i1];
          }

          EEPROM.put(indexAddress, index);
          EEPROM.put(arrayAddress, card_arr);
          Serial.print("Index :");
          Serial.println(index);
          display_done();
          flush_Serial1();
        }
      }
    }


    else if (index == 0 && master_status == 0 && resetKey_status != 1) {
      display_invalid();
      Serial.println("Invalid Card index 0");
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
        //Serial.println("Invalid");
        display_invalid();
        Serial.println("invalid card Master_S 0 index>0");
        code = 0;
        delay(3000);
      }

      else if (code == 1) {
        if (off_on == 0 && millis() - timer > 30000) {
          if (disp == 0) {
            Serial.println("Authenticated off");
            display_authenticated();
            disp = 1;
          }
          digitalWrite(buzzer, HIGH);
          shutdown_sequence();  // has a delay 2s
          delay(2000);
          digitalWrite(buzzer, LOW);
          delay(3000);
          display_solvision();
          //delay(shutdown_time * 60000000);
          //delay(5000); //delay for testing
          display_timer();
          digitalWrite(relay, LOW);
          digitalWrite(LED_Green, LOW);
          timer = millis();
          off_on = 1;
        }

        if (off_on == 1 && millis() - timer > 30000) {
          Serial.println("off_on=1 Entered");
          digitalWrite(LED_Green, HIGH);
          digitalWrite(relay, HIGH);
          digitalWrite(buzzer, HIGH);
          off_on = 0;
          timer = millis();
          if (disp == 0) {
            Serial.println("Authenticated on");
            display_authenticated();
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


void shutdown_sequence() {
  Serial.println("Shutdown Sequence");
  Keyboard.press(KEY_LEFT_GUI);
  delay(500);
  Keyboard.press('x');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.press('u');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.press('u');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.press('u');
  Keyboard.releaseAll();
}

void display_solvision() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.println("SOLVISION");
  display.display();
}

void display_master() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 22);
  display.println("MASTER KEY");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.println("Put Card");
  display.display();
}

void display_processing() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 22);
  display.println("Processing");
  display.display();
}

void display_done() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.println("Done");
  display.display();
  delay(2000);
}

void display_invalid() {
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 22);
  display.println("Invalid Card");
  display.display();
  flush_Serial1();
}

void display_authenticated() {
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 22);
  display.println("Authenticated");
  display.display();
  disp = 1;
}

void display_dublicate() {
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 22);
  display.println("Dublicate Key");
  display.display();
  delay(2000);
}

void display_resetDone() {
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 22);
  display.println("Erasing Keys");
  display.display();
  delay(2000);
  disp=1;
}

void display_starting() {
  for (int i = 0; i < 5; i++) {
    display.clearDisplay();
    display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 22);
    Serial.println("Starting.");
    display.println("Starting.");
    display.display();

    delay(200);
    display.clearDisplay();
    display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 22);
    Serial.println("Starting..");
    display.println("Starting..");
    display.display();

    delay(200);
    display.clearDisplay();
    display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 22);
    Serial.println("Starting...");
    display.println("Starting...");
    display.display();
    delay(200);
  }
}

void display_timer() {
  for (int minutes = 2; minutes >= 0; minutes--) {
    for (int seconds = 59; seconds >= 0; seconds--) {
      display.clearDisplay();
      display.setCursor(30, 25);
      if (minutes < 10) {
        display.print("0");  // Add leading zero for single-digit minutes
      }
      display.print(minutes);
      display.print(":");
      if (seconds < 10) {
        display.print("0");  // Add leading zero for single-digit seconds
      }
      display.print(seconds);
      display.display();
      delay(1000);  // Update display every second
    }
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
  Serial.println("Flush Clear");
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
