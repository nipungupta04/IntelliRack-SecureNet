#include <SoftwareSerial.h>
#include <Keyboard.h>

// SoftwareSerial myserial(2, 3); // SoftwareSerial pins
int card_no[12] = {0, 0, 0, 0, 8, 7, 6, 5, 4, 3, 2, 1};
bool received_confirmation = false;
int k = 0;

void setup() {
  Serial.begin(9600);
  // myserial.begin(9600);
  delay(5000);
  Serial.println("Log Sequence");
  delay(500);
  Keyboard.press(KEY_LEFT_GUI);
  delay(500);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("cmd");
  delay(1000);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();
  delay(1200);
  Keyboard.println("cd documents");
  delay(1000);
}

void loop() {
  if (!received_confirmation) {
    k++;
    String data = "datalogger_xls_Conf.exe \"";
    for (int i = 0; i < 4; i++) {
      data += String(card_no[i],HEX);
      if (i != 3)
        data += " ";
    }
    data += ",open";
    data += ",Serial\"";
    Keyboard.println(data);
    // myserial.println(data);
    delay(3000);

  }

  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == 'R') {
      received_confirmation = true;
      Serial.println("Received confirmation 'R'.");
    }
  }
}
