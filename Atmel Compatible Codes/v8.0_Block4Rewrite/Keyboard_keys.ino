void log_sequence( String on_close, int override = 0) {
  if ((log_status == 0 && millis() - timer > 45000) || override == 1)
  {
    Serial.println("Log Sequence");
    delay(500);
    Keyboard.press(KEY_LEFT_GUI);
    delay(500);
    digitalWrite(buzzer, LOW);
    Keyboard.press('r');
    Keyboard.releaseAll();
    delay(500);
    Keyboard.print("cmd");
    delay(500);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    delay(1500);
    Keyboard.println("cd documents");
    delay(800);
    Keyboard.println("python datalogger_xls_Conf.py");
    delay(1200);
    String data = "";
    for (int i = 0; i < 4; i++) {
      data += String(uid_Auth[i],HEX);
      data+="  ";
    }
    data += ',';
    data += on_close;
    data += ",SR-RV 20230002";
    recieved_confirmation = false;
    Serial.println(data);
    int i = 0;
    for (i = 0; i < 4; i++)
    {
      if (Serial.available() > 0) {
        char receivedChar = Serial.read();
        if (receivedChar == 'R') {
          recieved_confirmation = true;
        }
        if (!recieved_confirmation)
          Serial.println(data);
        delay(100);
      }
    }
    delay((4 - i) * 100);
    Keyboard.println("exit");
    log_status = 1;
    delay(200);
  }
}

void shutdown_sequence() {
  log_sequence("close", 1);
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
