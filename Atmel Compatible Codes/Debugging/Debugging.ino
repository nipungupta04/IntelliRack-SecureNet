#include <EEPROM.h>
const int MAX_INDEX = 10;
const int ARRAY_LENGTH = 12;
int indexAddress = 0;
int arrayAddress = sizeof(int);
int index;
int card_arr [MAX_INDEX][ARRAY_LENGTH];
int card_no[12];
int count = 0;
int flag = 1;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  EEPROM.get(indexAddress, index);
  EEPROM.get(arrayAddress, card_arr);
  if (index > 6 || index < 0)
  {
    index = 0;
    EEPROM.write(0, index);
  }
  Serial.print("index at start: ");
  Serial.println(index);

  for (int k = 0; k < index; k++)
  {
    for (int i = 0; i < 12; i++)
    {
      Serial.print(card_arr[k][i]);
    }
    Serial.println();
    delay(100);
  }
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
      card_arr[index][i]=card_no[i];
    }
    flag = 1;
    Serial.println();
    index++;
    EEPROM.put(indexAddress, index);
    EEPROM.put(arrayAddress, card_arr);
    delay(1000);
    Serial.println("GO...");
  }/*
  if (Serial1.available()) {
    Serial.println(Serial1.read();
  }*/

}
