#include <Adafruit_NeoPixel.h>
#include <SensorQMI8658.hpp>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

#define QMI_ADDRESS 0x6B
#define QMI8658C_I2C_FREQUENCY 80000

int RGB_LED_POWER_PIN = 7;
int RGB_LED_DATA_PIN = 8;

int TFT_DC_PIN = 36;
int TFT_CS_PIN = 35;
int TFT_RST_PIN = 34;
int TFT_BACKLIGHT_PIN = 33;
int SCK_PIN = 40;
int MOSI_PIN = 38;
int MISO_PIN = 39;

int SDA_PIN = 12;
int SCL_PIN = 11;

int BUTTON_B_PIN = 0;
int BUTTON_R_PIN = 47;
int BUTTON_G_PIN = 48;

int IR_PIN = 9;

int INTERRUPT1_PIN = 18;
int INTERRUPT2_PIN = 21;

//pro RGB LED, mame jen jednu, proto ta 1 na zacatku konstruktoru
Adafruit_NeoPixel pixels(1, RGB_LED_DATA_PIN, NEO_RGB + NEO_KHZ800);

//pro TFT displej, musi se nastavit vsechny piny, jinak to nefunguje
Adafruit_GC9A01A tft(TFT_CS_PIN, TFT_DC_PIN, MOSI_PIN, SCK_PIN, TFT_RST_PIN, MISO_PIN);

//akcelerometr, gyroskop, teplmer
SensorQMI8658 qmi;
IMUdata acc;
IMUdata gyr;

void setup() {

  Serial.begin(115200);

  //Zapnout napajeni do RGB LED
  pinMode(RGB_LED_POWER_PIN, OUTPUT);
  digitalWrite(RGB_LED_POWER_PIN, HIGH);

  //Zapnout podsviceni displeje
  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(TFT_BACKLIGHT_PIN, HIGH);  // Backlight on

  //Nastavit tlacitka - obracena logika, kdyz neni zmacknuto, digitalRead vraci 1, kdyz zmacknu, vraci 0
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_R_PIN, INPUT_PULLUP);
  pinMode(BUTTON_G_PIN, INPUT_PULLUP);

  pixels.begin();
  tft.begin();

  if (!qmi.begin(Wire, 0x6B, SDA_PIN, SCL_PIN)) {
    Serial.println("QMI8658 Nenalezen");
  } else {
    Serial.print("Device ID:");
    Serial.println(qmi.getChipID(), HEX);

    //Nastaveni, prevzato z ukazkove knihovny
    qmi.configAccelerometer(
      SensorQMI8658::ACC_RANGE_4G,
      SensorQMI8658::ACC_ODR_1000Hz,
      SensorQMI8658::LPF_MODE_0,
      true);

    qmi.configGyroscope(
      SensorQMI8658::GYR_RANGE_64DPS,
      SensorQMI8658::GYR_ODR_896_8Hz,
      SensorQMI8658::LPF_MODE_3,
      true);

    qmi.enableGyroscope();
    qmi.enableAccelerometer();
    qmi.dumpCtrlRegister();
  }
}

int valR;
int valG;
int valB;
int color;
String qmiData;

void loop() {


  //Podle barvy stisknuteho tlacitka rozsvitime RGB led prislusnou barvou, barvy lze kombinovat
  valR = digitalRead(BUTTON_R_PIN);
  valG = digitalRead(BUTTON_G_PIN);
  valB = digitalRead(BUTTON_B_PIN);

  Serial.println("RGB buttons: " + String(valR) + "|" + String(valG) + "|" + String(valB));

  //barva podle stisknuteho tlacitka - rozsviti LED a nastavi i barvu textu na displej
  color = pixels.Color(valR == 0 ? 128 : 0, valG == 0 ? 128 : 0, valB == 0 ? 128 : 0);

  pixels.setPixelColor(0, color);
  pixels.show();


  //preceteme data ze senzoru a vypiseme, i na displej
  if (qmi.getDataReady()) {

    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
      qmiData = "ACC x: " + String(acc.x);
      displayText(4, 4, qmiData, color, true);
      qmiData = "ACC y: " + String(acc.y);
      displayText(4, 10, qmiData, color, false);
      qmiData = "ACC z: " + String(acc.z);
      displayText(4, 20, qmiData, color, false);
    }

    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
      qmiData = "GYR x: " + String(gyr.x);
      displayText(4, 40, qmiData, color, false);
      qmiData = "GYR y: " + String(gyr.y);
      displayText(4, 50, qmiData, color, false);
      qmiData = "GYR z: " + String(gyr.z);
      displayText(4, 60, qmiData, color, false);
    }

    qmiData = "TMP: " + String(qmi.getTemperature_C()) + " *C";
    displayText(4, 80, qmiData, color, false);
  }


  delay(1000);
}

//vykresli text na souradnici urcitou barvou (z Neopixel knihovny), lze nastavit jestli pred vykreslenim vymazat obrazovku
void displayText(int x, int y, String text, int textColor, bool blankScreen) {

  tft.setRotation(0);

  if (blankScreen) {
    tft.fillScreen(GC9A01A_BLACK);
  }

  //pokud prisel cerny text, dame bily, protoze mame cerne pozadi
  if (textColor == 0) {
    textColor = GC9A01A_WHITE;
  }


  tft.setCursor(x, y);
  tft.setTextColor(convertColor(textColor));
  tft.setTextSize(1);
  tft.println(text);
  Serial.println(text);
  Serial.println(textColor);
}

//prevede 32 bit barvu z neopixelu na 16 bit barvu pro TFT displej
int16_t convertColor(int32_t neoPixelColor) {
  return tft.color565((neoPixelColor >> 16) & 0xFF, (neoPixelColor >> 8) & 0xFF, neoPixelColor & 0xFF);
}
