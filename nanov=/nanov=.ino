#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

struct AuthorizedCard {
  byte uid[4];
  bool isVoteGiven; // New boolean field to track whether the vote has been given by this card
};

int abc = 0;
int def = 0;

// Replace these with the UIDs of your authorized RFID cards
AuthorizedCard authorizedCards[] = {
  { {0x53, 0x2C, 0x90, 0x56}, false },
  { {0x93, 0xCD, 0xF4, 0x56}, false }
};

// Define the I2C address of your LCD (you can find it on the back of the LCD module)
int lcdAddress = 0x27;

// Define the number of columns and rows on your LCD
int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize the LCD
  lcd.begin();
  lcd.backlight();

  lcd.print("Scan Your card");
}

void loop() {
  if (digitalRead(3) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("abc has ");
    lcd.print(abc);
    lcd.print(" Vote");

    // Assuming abc and def are variables declared and assigned values
    lcd.setCursor(0, 1);
    lcd.print("def has ");
    lcd.print(def);
    lcd.print(" Vote");

    // Print "Pressed" to the Serial Monitor

    // Add a delay to avoid rapid multiple readings
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Your card");
    // Adjust the delay as needed
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Give your vote");

    if (isAuthorized(mfrc522.uid)) {
      int a = vote();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Thank you!");
      delay(1000);
    } else {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Access Denied!");
      Serial.println("Access Denied - Not Matched!");
    }

    delay(3000); // Display the result for 3 seconds
    lcd.clear();
    lcd.print("Scan Your card!");
    delay(1000); // Delay to avoid rapid card reading
  }
}

bool isAuthorized(MFRC522::Uid cardUID) {
  for (int i = 0; i < sizeof(authorizedCards) / sizeof(authorizedCards[0]); i++) {
    if (compareUID(cardUID, authorizedCards[i].uid)) {
      // Check if the vote has already been given by this card
      if (authorizedCards[i].isVoteGiven) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Vote already given!");
        delay(3000);
        lcd.clear();
        lcd.print("Scan Your card!");
        delay(1000);
        return false;
      } else {
        return true;
      }
    }
  }
  return false;
}

bool compareUID(MFRC522::Uid uid1, byte uid2[]) {
  if (uid1.size != 4) {
    return false;
  }

  for (byte i = 0; i < uid1.size; i++) {
    if (uid1.uidByte[i] != uid2[i]) {
      return false;
    }
  }

  return true;
}

int vote() {
  bool ist = true;
  while (ist) {
    if (digitalRead(7) == HIGH) {
      lcd.setCursor(0, 1);
      abc++;

      // Set the isVoteGiven flag to true for the authorized card
      setVoteGivenFlag(mfrc522.uid, true);

      // Print "Pressed" to the Serial Monitor
      return 1;
      ist = false;
      // Add a delay to avoid rapid multiple readings
      delay(1000); // Adjust the delay as needed
    }
    if (digitalRead(2) == HIGH) {
      lcd.setCursor(0, 1);
      def++;

      // Set the isVoteGiven flag to true for the authorized card
      setVoteGivenFlag(mfrc522.uid, true);

      // Print "Pressed" to the Serial Monitor
      return 1;
      ist = false;
      // Add a delay to avoid rapid multiple readings
      delay(1000); // Adjust the delay as needed
    }
  }
}

void setVoteGivenFlag(MFRC522::Uid cardUID, bool value) {
  for (int i = 0; i < sizeof(authorizedCards) / sizeof(authorizedCards[0]); i++) {
    if (compareUID(cardUID, authorizedCards[i].uid)) {
      authorizedCards[i].isVoteGiven = value;
      return;
    }
  }
}
