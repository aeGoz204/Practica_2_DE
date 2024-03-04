/****************************************
 * Libraries
 ****************************************/
#include <UbidotsEsp32Mqtt.h>
#include "DHT.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "xmb.h"

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-Dcrq35CuJZHXkBgjCxZ84sUCGg8fjK";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "TIGO-357A";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "2NB112101810";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL2 = "Humedad"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL1 = "Temp";
const char *SUBSCRIBE_SW1 = "SW1";
const char *SUBSCRIBE_SW2 = "SW2";

const uint8_t LED = 26;
const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
#define DATAPIN 27
#define DHTTYPE DHT11


Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht (DATAPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String str = String(topic);
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println(str.substring(20));
  if (str.substring(20) == "sw1/lv"){
    if ((char)payload[0] == '1')
    {
      tft.fillCircle(40, 125, 20, TFT_GREEN);
      digitalWrite(LED, HIGH);
    }
    else{
      tft.fillCircle(40, 125, 20, TFT_RED);
      digitalWrite(LED, LOW);
    }
  }  if (str.substring(20) == "sw2/lv"){
        if ((char)payload[0] == '1')
    {
      tft.fillCircle(95, 125, 20, TFT_NAVY);
    }
    else{
      tft.fillCircle(95, 125, 20, TFT_RED);
    }
  }
  
}

void AppleLogo(int x, int y){
  delay(200);
  tft.drawXBitmap(x, y, apple, logoWidth, logoHeight, TFT_WHITE);
  delay(1000);
  tft.drawXBitmap(x, y, apple, logoWidth, logoHeight, TFT_BLACK); 
  delay(250);
}
void UI(void){
 tft.drawRect(0, 0, 135, 240, TFT_WHITE);
 tft.fillRect(40, 125, 21, 21, TFT_WHITE);
 tft.fillRect(95, 125, 21, 21, TFT_WHITE);
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  pinMode(LED, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_SW1);
  ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_SW2);
  
  int x = 22, y = 85;
  tft.init();
  tft.fillScreen(TFT_BLACK);
  AppleLogo(x, y);
  UI();
  tft.drawString(F("Humedad:"), 5, 20, 4);
  tft.drawString(F("Temp.:"),   5, 155, 4);
  tft.drawString(F("%"),      100, 55 , 4);
  tft.drawString(F("`C"),     100, 190, 4);
  
  dht.begin();
  
  timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  tft.setTextColor(TFT_WHITE);
  tft.drawString( String(h, 1), 5, 55 , 6);
  tft.drawString( String(t, 1), 5, 190, 6);
  
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_SW1);
    ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_SW2);
  }
  if ((millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    ubidots.add(VARIABLE_LABEL1, t); // Insert your variable Labels and the value to be sent
    ubidots.add(VARIABLE_LABEL2, h);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
  delay(1000);
  tft.setTextColor(TFT_BLACK);
  tft.drawString( String(h, 1), 5, 55 , 6);
  tft.drawString( String(t, 1), 5, 190, 6);
  
}
