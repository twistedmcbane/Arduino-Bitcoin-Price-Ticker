# Arduino-Bitcoin-Price-Ticker
Arduino Bitcoin Price Ticker

  Clone this TFT library to you Arduino library folder (e.g. "C:\Users\twist\OneDrive\Dokumente\Arduino\libraries")
  https://github.com/Bodmer/TFT_eSPI.git

  I had to make 2 small changes to get it working for my display:
  -User_Setup_Select.h:
    line 22: //#include <User_Setup.h> // Comment out User_Setup.h
    line 25: #include <User_Setups/Setup2_ST7735.h> // Uncomment this header file
  
  -User_Setups/Setup2_ST7735.h:
    line 10: #define ST7735_BLACKTAB
  


TFT Display (ST7735):
https://www.amazon.de/gp/product/B078JBBPXK/ref=ppx_yo_dt_b_asin_title_o00__o00_s00?ie=UTF8&psc=1

Arduino NodeMcu v3:
https://www.amazon.de/gp/product/B074Q2WM1Y/ref=ppx_yo_dt_b_asin_title_o03__o00_s00?ie=UTF8&psc=1

Connection:

TFT(ST7735) -->	NodeMcu v3

GND		      -->   G

VCC		      -->	  3V

SCK		      -->	  D5

SDA		      -->	  D7

RES		      -->	  D4

RS		      -->	  D3

CS		      -->	  D8

LEDA	      -->	  3V

