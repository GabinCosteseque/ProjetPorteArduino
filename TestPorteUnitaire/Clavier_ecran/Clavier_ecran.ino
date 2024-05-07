#include <SoftwareSerial.h>
#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;

SoftwareSerial mySerial(0, 2); // Définir un objet SoftwareSerial sur les broches 2 (RX) et 14 (TX)
int Signal= 14;
int BP = 16;
const int colorR = 200;
const int colorG = 200;
const int colorB = 200;
String CodeValidation = "1234";


void setup() {
    pinMode(BP, INPUT);

    pinMode(Signal, OUTPUT);    
    digitalWrite(Signal, HIGH);
    mySerial.begin(9600);  // Initialiser la communication série logicielle avec le module
    Serial.begin(9600);  // Initialiser la communication série avec le moniteur série
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);

}

void loop() {
  String Message = "";
  if(digitalRead(BP) == HIGH)
  {
  digitalWrite(Signal, LOW);
  delay(3000);
  digitalWrite(Signal, HIGH);
  }
  
  else {
    printData();
  }
}

void printData() {
            
    static String input = ""; // Variable pour stocker les chiffres entrés (déclarée comme static)
    if (mySerial.available()) {
        uint8_t data = mySerial.read();
        if (data == 0xEA) { // Si la touche "*" est pressée (0xEA)
            if (input.length() > 0) { // Vérifier si la chaîne d'entrée n'est pas vide
                input.remove(input.length() - 1); // Supprimer le dernier caractère de la chaîne d'entrée
                lcd.clear(); // Effacer l'écran LCD
                lcd.print(input); // Afficher la chaîne d'entrée mise à jour sur l'écran LCD
            }
        } else if (input.length() < 4 && data >= 0xE1 && data <= 0xE9) { // Attendre d'avoir 4 chiffres et traiter les chiffres entrés
            input += (char)(data - 0xE0 + '0'); // Convertir le code en chiffre ASCII
            lcd.clear(); // Effacer l'écran LCD
                lcd.print(input);
        } 
        else if (input.length() < 4 && data == 0xEB) { // Attendre d'avoir 4 chiffres et traiter les chiffres entrés
            input += '0'; // Convertir le code en chiffre ASCII
            lcd.clear(); // Effacer l'écran LCD
                lcd.print(input);
        }else if (data == 0xEC && input.length() == 4) { // Si la touche "#" est pressée et 4 chiffres ont été entrés
            // Effectuer une action de validation ici
            lcd.setCursor(0,1);
            lcd.print("Analyse...");
            delay(2000);
            Serial.println(input);
            if (input == CodeValidation){ 
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.setRGB(0, 255, 0);
              lcd.print("Code correct !");
              Serial.println("Code correct: " + input);
              digitalWrite(Signal, LOW);
              delay(2000);
              digitalWrite(Signal, HIGH);
            }
            else
            {
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Code incorrect !");
              lcd.setRGB(255, 0, 0);
              Serial.println("Code incorrect: " + input);
              delay(2000);
            }
             // Réinitialiser la variable d'entrée pour la prochaine saisie
             input = "";
            lcd.setRGB(colorR, colorG, colorB);      
            lcd.clear(); // Effacer l'écran LCD
            
            
        }
    }
}
