#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NfcAdapter.h>
#include <PN532/PN532_SWHSU/PN532_SWHSU.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <rgb_lcd.h>
rgb_lcd lcd;
SoftwareSerial nfcmodule(12, 13);
PN532_SWHSU pn532swhsu(nfcmodule);
NfcAdapter nfc(pn532swhsu);
String Route0 = "http://192.168.102.84:8080/serrure/badge";
String Route1 = "http://192.168.102.84:8080/serrure/auth";
String ID = "Camping";
String MDP = "camping31";
SoftwareSerial mySerial(0, 2);
int Signal = 14;
int BP = 16;
const int colorR = 200;
const int colorG = 200;
const int colorB = 200;
String TokenAPI = "1f4aae087b87c21100fa9a1a140f9f24";
String TokenDoor = "6b86b273ff34fce19d6b804eff5a3f57";
String input = "";

void setup() 
{
  mySerial.begin(9600);
  WiFi.begin(ID, MDP);
  pinMode(Signal, OUTPUT);
  pinMode(BP, INPUT);
  digitalWrite(Signal, HIGH);
  Serial.begin(9600);
  nfc.begin();
  lcd.begin(16, 2);  // Initialisation de l'écran LCD
  lcd.setRGB(colorR, colorG, colorB);  // Définir la couleur de l'écran LCD
}

void loop() 
{
  String Code;
  String Reponse_API = "Skip";

  if (nfc.tagPresent()) 
  {
    Code = Badge();
    Serial.println(Code);
    Reponse_API = Envoi_Code_API(Code, Route0);
    delay(500);
  }
  else if (mySerial.available()) 
  {
    Code = Clavier();
    if (Code != "Skip")
    {
    Reponse_API = Envoi_Code_API(Code, Route1);
    input = "";
     }
  }
  else
  {
    Lecture_Bouton();
  }

  if (Reponse_API == "0201")
  {
    CODE_VALID();
  }
  else if (Reponse_API == "0202")
  {
    CODE_INVALID();
  }
  else if (Reponse_API != "Skip")
  {
    CODE_ERROR(Reponse_API);
  }
}

String Badge() {
  String code;
  String result;
  NfcTag tag = nfc.read();
  code = tag.getUidString();
  for (int i = 0; i < code.length(); i++) {
    if (code[i] != ' ') {
      result += code[i];
    }
  }
  return result;
}


String Clavier() 
{
    uint8_t data = mySerial.read();
        if (input.length() < 6 && data >= 0xE1 && data <= 0xE9) 
    {
      input += (char)(data - 0xE0 + '0');
      lcd.clear();
      lcd.print(input);
    } 
    else if (input.length() < 6 && data == 0xEB) 
    {
      input += '0';
      lcd.clear();
      lcd.print(input);
    } 
    else if ((data == 0xEA) && (input.length() > 0)) 
    {
        input.remove(input.length() - 1);
        lcd.clear();
        lcd.print(input);
    } 
    else if ((data == 0xEC)  && (input.length() == 6)) 
    {
      lcd.setCursor(0, 1);
      lcd.print("Analyse...");
      delay(2000);
      lcd.clear();
      return input;
    }
  return "Skip";
}

String Envoi_Code_API(String Code, String Route) 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    WiFiClient client;
    HTTPClient http;
    DynamicJsonDocument doc(1024);
    if (Route == Route0)
    {
    http.begin(client, Route0);  
    doc["Token"] = TokenDoor;
    doc["Badge"] = Code;
    }
     if (Route == Route1)
    {
    http.begin(client, Route1);
    doc["Token"] = TokenDoor;
    doc["Code"] = Code;
    }
    http.addHeader("Content-Type", "application/json");
    String requestBody;
    serializeJson(doc, requestBody);
    int httpCode = http.POST(requestBody);
    if (httpCode > 0) 
    {
      if (httpCode == HTTP_CODE_OK) 
      {
        const String& payload = http.getString();
        deserializeJson(doc, payload);
        String Reponse = String(doc["info"].as<const char*>());
        Serial.print("Code reponse de l'api :");
        Serial.println(Reponse);
        String TokenAPI2 = String(doc["token"].as<const char*>());
        Serial.print("Token de l'api :");
        Serial.println(TokenAPI2);
        http.end();
        if (TokenAPI == TokenAPI2 )
        {
          return Reponse;
        }
      }
    }
    http.end(); 
  }
  return "Skip";
}

void Lecture_Bouton()
{
  bool lectureBP = digitalRead(BP);
  if (lectureBP == HIGH)
  {
    digitalWrite(Signal, LOW);
    delay(2000);
    digitalWrite(Signal, HIGH);
  } 
}

void CODE_VALID()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setRGB(0, 255, 0);
  lcd.print("Code correct !");
  digitalWrite(Signal, LOW);
  delay(2000);
  digitalWrite(Signal, HIGH);
  lcd.setRGB(colorR, colorG, colorB);      
  lcd.clear();
}

void CODE_INVALID()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setRGB(255, 0, 0);
  lcd.print("Code incorrect !");
  delay(2000);
  lcd.setRGB(colorR, colorG, colorB);      
  lcd.clear();
}

void CODE_ERROR(String Reponse_API)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setRGB(255, 0, 0);
  lcd.print("ERREUR");
  lcd.setCursor(0, 1);
  lcd.print(Reponse_API);
  delay(2000);
  lcd.setRGB(colorR, colorG, colorB);      
  lcd.clear();
}
