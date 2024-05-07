
#include <NfcAdapter.h>
#include <PN532/PN532_SWHSU/PN532_SWHSU.h>
#include <SoftwareSerial.h>
#include <Wire.h>


SoftwareSerial nfcmodule(12,13);
PN532_SWHSU pn532swhsu(nfcmodule);
NfcAdapter nfc(pn532swhsu);

int Signal= 14;
int BP = 16;
String CodeBadge = "w9NqPvA3uLz7XrE4m2oFfB5iCcYtGxV1hSdW8jM6nZpKy0lOgRbQaH";



void setup() {
    pinMode(BP, INPUT);

    pinMode(Signal, OUTPUT);    
    digitalWrite(Signal, HIGH);

    Serial.begin(9600);  // Initialiser la communication série avec le moniteur série

    nfc.begin();
}

void loop() {
  String Message = "";
  if(digitalRead(BP) == HIGH)
  {
  digitalWrite(Signal, LOW);
  delay(3000);
  digitalWrite(Signal, HIGH);
  }
  
  else if (nfc.tagPresent())
  {
     Message = getMessage();
     Serial.print(Message);
       if(Message == CodeBadge)
       {
        digitalWrite(Signal, LOW);
       delay(3000);
       digitalWrite(Signal, HIGH);
       }
  }
  Serial.print("1");
}


String getMessage() {
  String currentMessageContent = "";
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {
      NdefMessage nfcMessage = tag.getNdefMessage();
      for (int i = 0; i < nfcMessage.getRecordCount(); i++) {
        NdefRecord record = nfcMessage.getRecord(i);
        byte payload[record.getPayloadLength()];
        record.getPayload(payload);
        for (int c = 3; c < record.getPayloadLength(); c++) {
          currentMessageContent += (char)payload[c];
        }
      }
    }
  }
  return currentMessageContent;
}
