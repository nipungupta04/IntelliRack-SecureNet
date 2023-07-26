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
