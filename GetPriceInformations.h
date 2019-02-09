#include <CoinMarketCapApi.h>
const int numRetries = 10;

struct CoinInfo
{
  float price;
  float Percentage24Hr;
  const unsigned short* logo;
  
  CoinInfo():logo(NULL),price(-1.0f),Percentage24Hr(-1.0f)
  {
  }
};

const char* host = "api.bittrex.com";
const int httpsPort = 443;

const char fingerprint[] PROGMEM = "3C 57 98 13 5F 07 7D DD 51 27 67 A5 09 A7 C9 17 09 78 69 81";

CoinInfo GetPriceBTC()
{
  WiFiClientSecure client;
  CoinInfo ret;
  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return ret;
  }

  String url = "/api/v1.1/public/getmarketsummary?market=USD-BTC";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  String response;
  bool responseStart = false;
  String line;
  while (client.connected()) {
    //if( !responseStart )
    line = client.readStringUntil('\n');

    /*
      if (line == "\r") {
      Serial.println("headers received");
      break;
      }*/
    if ( responseStart && line.startsWith("{") )
      response = "[" + line + "]";
    //response += client.readStringUntil('\n');

    if (line == "\r") responseStart = true;
  }

  Serial.println( response );

  DynamicJsonBuffer jsonBuffer;
  JsonArray& root = jsonBuffer.parseArray(response);

  if (root.success()) {
    if ( root[0]["success"] == "true" )
    {
      Serial.println("Success");
      Serial.println( root[0]["result"].as<String>() );

      JsonArray& root2 = jsonBuffer.parseArray( root[0]["result"].as<String>() );
      ret.price = root2[0]["Last"].as<float>();
      ret.Percentage24Hr = (ret.price * 100.0f) / root2[0]["PrevDay"].as<float>() - 100.0f;
      Serial.println( ret.price );
      Serial.println( ret.Percentage24Hr );
    }
  }
  else
  {
    JsonObject& rootObject = jsonBuffer.parseObject(response);
    if (rootObject.containsKey("error"))
      Serial.println( rootObject["error"].as<String>() );
  }

  return ret;
}


const char fingerprintLND[] PROGMEM = "31fe28aa52444a112887a09cd421534a00216a49";

CoinInfo GetLightningStats()
{
  CoinInfo ret;

  int retries = 0;
  while( ret.price == -1 )
  {
    std::unique_ptr<BearSSL::WiFiClientSecure>clientSecure(new BearSSL::WiFiClientSecure);
    clientSecure->setFingerprint(fingerprintLND);
  
    HTTPClient https;
    if (https.begin(*clientSecure, "https://hashxp.org/lightning/node/"))
    { // HTTPS
  
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
  
      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = https.getString();
          //Serial.println(payload);
          String posString("Total channel capacity:BTC");
          int posStart = payload.indexOf( posString );
          int posEnd = payload.indexOf( "(USD" ,posStart + posString.length() );
          
          if( posStart >= 0 && posEnd >= 0 )
          {
            String capacity = payload.substring(posStart + posString.length(),posEnd);
       
            ret.price = atof( capacity.c_str() );
            Serial.println( ret.price );
          }
        }
        else
        {
          Serial.println("fail");
        }
      }
    }
    
    https.end();
    
    if( retries++ >= numRetries )
      return ret;
  }
  
  return ret;
}

const char fingerprintMarketwatch[] PROGMEM = "39 69 C9 96 4D 30 EC 40 46 43 6E 31 6D F3 DC D4 FE 7C 65 6D";

CoinInfo GetPriceMarketwatch(String Url)
{
  CoinInfo ret;
    
  int retries = 0;
  while( ret.price == -1 )
  {
    std::unique_ptr<BearSSL::WiFiClientSecure>clientSecure(new BearSSL::WiFiClientSecure);
    clientSecure->setFingerprint(fingerprintMarketwatch);
  
    HTTPClient https;
    if (https.begin(*clientSecure, Url))
    { // HTTPS
  
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
  
      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = https.getString();

          String priceString("<meta name=\"price\" content=\"");
          String percentString("<meta name=\"priceChangePercent\" content=\"");
          int pricepos = payload.indexOf( priceString );
          int priceposEnd = payload.indexOf("\"", pricepos + priceString.length());
          int percentpos = payload.indexOf( percentString );
          int percentposEnd = payload.indexOf("%", percentpos + percentString.length());
    
          if( pricepos > 0 && priceposEnd > 0 )
          {    
            String price = payload.substring(pricepos + priceString.length(),priceposEnd);
            ret.price = atof( price.c_str() );
          }
          if( pricepos > 0 && priceposEnd > 0 )
          {      
            String percent = payload.substring(percentpos + percentString.length(),percentposEnd);
            ret.Percentage24Hr = atof( percent.c_str() );
          }
        }
      }
    }
    https.end();
    
    if( retries++ >= numRetries )
      return ret;
  }
  
  return ret;
}
