#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keyboard.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int count = 0;
int card_no[12];
int test[12] = {51, 53, 48, 48, 52, 65, 55, 49, 66, 51, 66, 68};
unsigned long timer, timer1;
int flag = 1;
int off_on = 1; //0 mean on

#define shutdown_time 3

#define LED_Green 9   //pin 9
#define buzzer 10      //pin 10
#define relay 6       //pin 6

int disp = 0;

//Serial for computer
//Serial1 for pins Rx Tx
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  pinMode(LED_Green, OUTPUT);
  digitalWrite(LED_Green, LOW);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  display_starting(); //loop and delay inbuilt 5s

  timer1 = millis();
  timer = millis() + 300000;
  Serial.println("Begin...");
  display_solvision();
}

void loop() {
  display_solvision();
  if (off_on > 0)
    Serial.println("off");
  else
    Serial.println("on");

  Serial.print(timer);
  Serial.print(" ");
  Serial.println(millis());
  /* if (timer1 - millis() > 30000)
    {
     count = 0;
     timer1 = millis();
     //Serial.println("TIMER1 RESET");
    }*/

  if (Serial1.available())
  {
    disp = 0;
    card_no[count] = Serial1.read();
    count++;
    if (count == 12)
      flag = 0;
  }

  if (flag == 0)
  {
    for (int i = 0; i < 12; i++)
    {
      Serial.print(card_no[i]);
    }
    Serial.println();
    delay(3000);
    int code = 1;

    for (int i = 0; i < 12; i++)
    {
      if (card_no[i] != test[i])
      {
        Serial.println("Invalid");
        display_invalid();
        code = 0;
        delay(5000);
        break;
      }
    }

    if (code == 1)
    {
      if (off_on == 0 && millis() - timer > 30000)
      {
        if (disp == 0)
        {
          Serial.println("Authenticated off");
          display_authenticated();
          disp = 1;
        }

        shutdown_sequence();

        digitalWrite(buzzer, HIGH);
        delay(2000);
        digitalWrite(buzzer, LOW);
        delay(3000);
        display_solvision();
        delay(shutdown_time * 60000000);
        //delay(5000); //delay for testing
        digitalWrite(relay, LOW);
        digitalWrite(LED_Green, LOW);
        timer = millis();
        off_on = 1;
      }

      if (off_on == 1 && millis() - timer > 30000)
      {
        Serial.println("off_on=1 Entered");
        digitalWrite(LED_Green, HIGH);
        digitalWrite(relay, HIGH);
        digitalWrite(buzzer, HIGH);
        off_on = 0;
        timer = millis();
        if (disp == 0)
        {
          Serial.println("Authenticated on");
          display_authenticated();
          disp = 1;
        }
        delay(2000);
        digitalWrite(buzzer, LOW);
        delay(3000);
      }
    }

    flag = 1;
    count = 0;
  }
}


void display_solvision()
{
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.println("SOLVISION");
  display.display();
}

void shutdown_sequence()
{
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

void display_invalid()
{
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 22);
  display.println("Invalid Card");
  display.display();
}

void display_authenticated()
{
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 22);
  display.println("Authenticated");
  display.display();
  disp = 1;
}

void display_starting()
{
  for (int i = 0; i < 5; i++)
  {
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
