void log_sequence( String on_close, int override = 0) {
  if ((log_status == 0 && millis() - timer > 45000) || override == 1)
  {
    Serial.println("Log Sequence");
    delay(300);
    Keyboard.press(KEY_LEFT_GUI);
    delay(300);
    Keyboard.press('r');
    Keyboard.releaseAll();
    delay(300);
    Keyboard.print("cmd");
    delay(300);
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    delay(300);
    Keyboard.println("cd documents");
    delay(300);
    Keyboard.println("python datalogger3.py");
    delay(2000);
    String data = "";
    for (int i = 0; i < 12; i++) {
      data += String(card_no[i]);
    }
    data += ',';
    data += on_close;
    Serial.println(data);
    delay(200);
  }
}

void shutdown_sequence() {
  log_sequence("close",1);
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
