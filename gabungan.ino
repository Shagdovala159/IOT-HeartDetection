#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ThingSpeak.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#include <ArduinoJson.h>
#define I2CADDR 0x38

String apikey = "JSWC722EXUCIZZTU";  //Write Api Key
char ssid[] = "!";                   //Name WiFi
char pass[] = "anelita123";

const char *server = "api.thingspeak.com";
WiFiClient client;
unsigned long myChannelNumber = 2092697;
const char *myWriteAPIKey = "M2A3LS02Y5PHPTK0";
const char *myReadAPIKey = "F4N68KHKINC3LWWX";

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
// Digitran keypad, bit numbers of PCF8574 i/o port
byte rowPins[ROWS] = { 0, 1, 2, 3 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 4, 5, 6, 7 };  //connect to the column pinouts of the keypad
TwoWire *jwire = &Wire;               //test passing pointer to keypad lib
Keypad_I2C kpd(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574, jwire);

const int sensor =15;
const int pinBuzzer = 2;

long int beatCount = 0; 
int lastbeat; 
String kodepasien = "";
int mulai = 0;
unsigned long startMillis; 
unsigned long currentMillis; 
const unsigned long period = 60000; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //cod pada arduino
  Serial.println("Connected");
  while (!Serial) { /*wait*/
  }
  jwire->begin();
  kpd.begin();
  kpd.addEventListener(keypadEvent);
  Serial.println("Connected");
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  WiFi.begin(ssid, pass);
  pinMode(sensor, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  lastbeat = beatCount;
  digitalWrite(pinBuzzer, HIGH);
}

void loop() {
  char key = kpd.getKey();
  if (key) {
    if (key != '*' && key != '#') {
      Serial.print(key);
      kodepasien += key;
    }
  }
  if (mulai == 1) {
    digitalWrite(pinBuzzer, HIGH);
    if (digitalRead(sensor) == HIGH) {
      lastbeat = beatCount;
      beatCount++;
      if (lastbeat != beatCount) {
        digitalWrite(pinBuzzer, LOW);
        delay(50);
        digitalWrite(pinBuzzer, HIGH);
      }
      Serial.println(beatCount);
      delay(50);
    }
    currentMillis = millis();
    if (currentMillis - startMillis >= period) {
      Serial.print("Jumlah 1 menit : ");
      Serial.println(beatCount);
      digitalWrite(pinBuzzer, LOW);
      delay(50);
      digitalWrite(pinBuzzer, HIGH);
      digitalWrite(pinBuzzer, LOW);
      delay(50);
      digitalWrite(pinBuzzer, HIGH);
      digitalWrite(pinBuzzer, LOW);
      delay(50);
      digitalWrite(pinBuzzer, HIGH);
      ThingSpeak.writeField(myChannelNumber, 1, beatCount, myWriteAPIKey);
      updatedata();
      delay(10);
      beatCount = 0;
      mulai = 0;
      kodepasien = "";
      Serial.println(" ");
      Serial.println("SELESAI :D");
    }
  }
}

void keypadEvent(KeypadEvent key) {
  switch (kpd.getState()) {
    case PRESSED:
      switch (key) {
        case '*':
          Serial.println("\nMulai Deteksi :D");
              Serial.print("KODE PASIEN :");
          Serial.println(kodepasien);
          startMillis = millis();
          mulai = 1;
          break;
        case '#':
          Serial.println("\nRESET");
          kodepasien = "";
      }
  }
}

void getdata() {
  WiFiClient client;
  HTTPClient http;
  String address = "http://172.20.10.5/tugas_akhir/webapi/api/API.php?";
  address += "kodepasien=";
  address += kodepasien;
  http.begin(client, address);

  // Melakukan GET request
  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      // Parsing data JSON
      DynamicJsonDocument doc(1024);  // Ukuran buffer dapat disesuaikan sesuai kebutuhan
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.println("Failed to parse JSON");
      } else {
        // Mengambil nilai dari JSON
        String kodePasien = doc["kodepasien"].as<String>();
        String nama = doc["nama"].as<String>();
        int usia = doc["usia"].as<int>();
        int detakJantung = doc["detakjantung"].as<int>();

        // Menampilkan nilai-nilai yang diambil
        Serial.println("kodepasien : " + kodePasien);
        Serial.println("nama : " + nama);
        Serial.println("usia : " + String(usia));
        Serial.println("detakjantung : " + String(detakJantung));
      }
    }
  } else {
    Serial.println("Error in HTTP request");
  }
  http.end();
  delay(5000);  // Mengulangi setiap 5 detik
}

void updatedata() {
  WiFiClient client;
  HTTPClient http;
  String address = "http://172.20.10.5/tugas_akhir/webapi/api/API.php";
  http.begin(client, address);
  http.addHeader("Content-Type", "application/json");
  String payload = String("{\"kodepasien\":\"") + String(kodepasien) + String("\",\"detakjantung\":") + String(beatCount) + String("}");
  int httpResponseCode = http.PUT(payload);
  getdata();
  http.end();
  delay(5000);  // Mengulangi setiap 5 detik
}