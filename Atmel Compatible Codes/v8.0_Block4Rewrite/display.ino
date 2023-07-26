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
    char* msg = "Starting";
    display_toScreen(msg, 5, 2, 0);
    delay(200);
    msg = "Starting.";
    display_toScreen(msg, 5, 2, 0);
    delay(200);
    msg = "Starting..";
    display_toScreen(msg, 5, 2, 0);
    delay(200);
  }
}

void display_timer() {
  int minutes = 2;
  int seconds = 59;
  if (!recieved_confirmation) {
    seconds = 30;
    minutes = 0;
  }
  for (; minutes >= 0; minutes--) {
    if (minutes == 0 && seconds == 30)
      seconds = 30;
    else
      seconds = 59;
    for (; seconds >= 0; seconds--) {
      display.clearDisplay();
      display.setCursor(40, 25);
      if (minutes < 10) {
        display.print("0");
      }
      display.print(minutes);
      display.print(":");
      if (seconds < 10) {
        display.print("0");
      }
      display.print(seconds);
      display.display();
      delay(1000);
    }
  }
}
