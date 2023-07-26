void display_toScreen(const char* message, int x, int y, int delay_time = 0, int text_size = 2) {
  display.clearDisplay();
  if (text_size == 1)
    display.setFont(&FreeSerifItalic9pt7b);
  else
    display.setFont();
  display.setTextSize(text_size);
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
    //display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(5, 12);
    display.println("Starting");
    display.display();

    delay(200);
    display.clearDisplay();
    //display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(5, 12);
    display.println("Starting.");
    display.display();

    delay(200);
    display.clearDisplay();
    //display.setFont(&FreeMonoOblique9pt7b);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(5, 12);
    display.println("Starting..");
    display.display();
    delay(200);
  }
}

void display_timer() {
  int minutes = 2;
  int seconds = 59;
  if (!recieved_confirmation)
  {
    seconds = 30;
    minutes = 0;
  }
  for (; minutes >= 0; minutes--) {
    for (; seconds >= 0; seconds--) {
      display.clearDisplay();
      display.setCursor(40, 25);
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
