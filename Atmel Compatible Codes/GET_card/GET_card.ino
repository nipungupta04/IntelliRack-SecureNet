int card_no[12];
int count = 0;
int flag = 1;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); 
}

void loop() {
  if (Serial1.available()) {
    card_no[count] = Serial1.read();
    count++;
    if (count == 12) {
      flag = 0;
      count = 0;
    }
  }
  
  if (flag == 0) {
    for (int i = 0; i < 12; i++)
    {
      Serial.print(card_no[i]);
    }
    flag = 1;
    Serial.println();
    delay(1000);
  }/*
  if (Serial1.available()) {
    Serial.println(Serial1.read();
  }*/

}
