void display_toScreen_mono(const char* message, int x, int y, int delay_time = 0) {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(message);
  display.display();
  delay(delay_time * 1000);
  disp = 1;
}

void display_toScreen_serif(const char* message, int x, int y, int delay_time = 0) {
  display.clearDisplay();
  display.setFont(&FreeSerifItalic9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(message);
  display.display();
  delay(delay_time * 1000);
  disp = 1;
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
