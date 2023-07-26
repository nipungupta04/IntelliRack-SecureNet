void readNFC()
{
  //flush_Serial1();
  uint8_t uid[] = {0, 0, 0, 0 };
  uint8_t success;                          // Flag to check if there was an error with the PN532
  uint8_t currentblock;                     // Counter to keep track of which block we're on
  bool authenticated_bool = false;               // Flag to indicate if the sector is authenticated
  //uidLength=0;
  uint8_t keyuniversal[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  if (success) {
    if (uidLength == 4)    {
      currentblock = 4;
      if (nfc.mifareclassic_IsFirstBlock(currentblock)) authenticated_bool = false;
      if (!authenticated_bool) {
        success = nfc.mifareclassic_AuthenticateBlock (uid, uidLength, currentblock, 1, keyuniversal);
        if (success) {
          authenticated_bool = true;
        }
      }
      if (authenticated_bool) {
        success = nfc.mifareclassic_ReadDataBlock(currentblock, data);
        Authenticated_card = true;
        for (int i = 0; i < 4; i++) {
          if (data[i] != Authentication_bits[i])  {
            //Serial.print(data[i]);
            Authenticated_card = false;
            const char* msg = "Invalid Card";
            display_toScreen(msg, 10, 20, 2, 1);
            delay(2000);
            return;
          }
        }
        if (Authenticated_card) {
          int k = 0;
          for (int i = 4; i < 8; i++) {
            uid_Auth[k] = data[i];
            k++;
            flag = 0;
            disp = 0;
          }
        }
      }
    }
  }
  delay(100);
}
