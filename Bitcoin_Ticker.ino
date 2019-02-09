#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "Logos.h"
#include "GetPriceInformations.h"
#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();

const char* ssid     = "Put Your SSID Here";
const char* password = "Put Your Password Here";

void setup()
{
  Serial.begin(115200);
  
  tft.begin();

  // Clear the buffer.
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0);
  
  tft.println("Booting");
}

void PrintWlanStatus()
{
  switch (WiFi.status())
  {
    case (WL_CONNECTED):
      Serial.println("WL_CONNECTED");
      break;
    case (WL_NO_SHIELD):
      Serial.println("WL_NO_SHIELD");
      break;
    case (WL_IDLE_STATUS):
      Serial.println("WL_IDLE_STATUS");
      break;
    case (WL_NO_SSID_AVAIL):
      Serial.println("WL_NO_SSID_AVAIL");
      break;
    case (WL_SCAN_COMPLETED):
      Serial.println("WL_SCAN_COMPLETED");
      break;
    case (WL_CONNECT_FAILED):
      Serial.println("WL_CONNECT_FAILED");
      break;
    case (WL_CONNECTION_LOST):
      Serial.println("WL_CONNECTION_LOST");
      break;
    case (WL_DISCONNECTED):
      Serial.println("WL_DISCONNECTED");
      break;
  }
}

void connectWlan()
{
  int repeats = 20;

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0);
  
  tft.println("Connecting to ");
  tft.println(ssid);

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    tft.print(".");
    //WiFi.printDiag(Serial);
    PrintWlanStatus();
    if ( repeats-- <= 0 )
      return;
  }

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  
  tft.println("");
  tft.println("WiFi connected");
  tft.println("IP address: ");
  tft.println(WiFi.localIP());

  return;
}

void DrawBitmap(const unsigned short* logo, int x, int y)
{
  tft.setSwapBytes(true);
  tft.pushImage(x, y, LOGO_WIDTH, LOGO_HEIGHT, logo);
}

void PrintFreeHeap()
{
  long  fh = ESP.getFreeHeap();
  Serial.println();
  Serial.println( "---FreeHeap---" );
  Serial.println( fh );
  Serial.println( "---FreeHeap---" );
  Serial.println();
}

float LNDCapacityStart = 0.0f;

CoinInfo infoBTC;
CoinInfo infoWDI;
CoinInfo infoBP;
CoinInfo infoLND;

void UpdateCoinInfo()
{
  infoLND = GetLightningStats();
  infoBP = GetPriceMarketwatch("https://www.marketwatch.com/investing/stock/bpe5?countrycode=xe");
  infoWDI = GetPriceMarketwatch("https://www.marketwatch.com/investing/stock/wdi?countrycode=xe");
  infoBTC = GetPriceBTC();

  infoBTC.logo = logo_BTC;
  infoLND.logo = logo_LND;
  infoBP.logo = logo_BP;
  infoWDI.logo = logo_WDI;

  if( LNDCapacityStart == 0.0f )
  {
    LNDCapacityStart = infoLND.price;
  }
  
  infoLND.Percentage24Hr = (infoLND.price * 100.0f) / LNDCapacityStart - 100.0f;
  //PrintFreeHeap();
  //infoETH = GetPriceETH();
  //PrintFreeHeap();
}

int timeC = 0;

void DrawPosition(int x,int y, CoinInfo& info)
{
  if( info.logo != NULL )
    DrawBitmap(info.logo,x,y);

  tft.setTextSize(1.5);
  tft.setCursor(x + 50, y + 6);
  tft.println(info.price);
  
  if( info.Percentage24Hr < 0 )
    tft.setTextColor(TFT_RED, TFT_BLACK);
  else
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.setCursor(x + 50, y + 16);
  tft.println( (String)info.Percentage24Hr + "%");

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void DrawScreen()
{
  tft.fillScreen(TFT_BLACK);
  int height = 160/4;
  int left = 10;
  int top = 4;
  
  DrawPosition(left, top, infoBTC);
  DrawPosition(left, top + height, infoLND);
  DrawPosition(left, top + 2*height, infoWDI);
  DrawPosition(left, top + 3*height, infoBP);
}

bool FirstRun = true;
void loop()
{
  while (WiFi.status() != WL_CONNECTED)
    connectWlan();

  if( FirstRun )
  {
    tft.fillScreen(TFT_BLACK);
    tft.println();
    tft.println("Updating Information...");
    FirstRun = false;
  }
  
  if ( timeC == 0 )
    UpdateCoinInfo();

  DrawScreen();

  for ( int i = 0 ; i < 600 ; i++ )
  {
    delay(100);
    timeC += 100;
    if ( timeC >= 60000 )
    {
      timeC = 0;
      return;
    }
  }
}
