#include <SoftwareSerial.h>
#include<Keyboard.h>
SoftwareSerial myserial(2, 3); // SoftwareSerial pins


void setup() {
  Serial.begin(9600);
  myserial.begin(9600);
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
  int card_no[12] = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  // Send the data to the computer via serial communication
  for (int k = 0; k < 10; k++)
  {
    for (int i = 0; i < 12; i++)
    {
      Serial.print(card_no[i]);
      myserial.print(card_no[i]);
    }
    myserial.print(',');
    delay(1000);
  }
  delay(2000);
  Keyboard.println("exit");
  delay(500);
}

void loop() {
  // Read sensor data or any other data you want to log

  // Adjust the delay as per your requirements
}
