#include <SoftwareSerial.h>
#include <Keyboard.h>

//SoftwareSerial myserial(2, 3); // SoftwareSerial pins
int card_no[12] = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

void setup() {
  Serial.begin(9600);
//  myserial.begin(9600);
  delay(5000);
  Serial.println("Log Sequence");
  delay(500);
  Keyboard.press(KEY_LEFT_GUI);
  delay(500);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("cmd");
  delay(500);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.println("cd documents");
  delay(500);
  Keyboard.println("python datalogger.py");
  delay(2000);

  delay(100);
}

void loop() {
  for (int k = 0; k < 10; k++) {
    String data = "";
    for (int i = 0; i < 12; i++) {
      data += String(card_no[i]);
    }
    data+=(",open");
    Serial.println(data);
    //myserial.println(data);
    delay(1000);
  }
}
