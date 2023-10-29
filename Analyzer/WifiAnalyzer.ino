#include <TFT_eSPI.h>
#include "WiFi.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite textscroller = TFT_eSprite(&tft);

#define color1 0xF79E
#define color2 0xB1B1
#define color3 0x4A69

String channel[11] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11" };
String dbm[7] = { "-30", "-40", "-50", "-60", "-70", "-80", "-90" };
int sum = 0,networkCount=0;
float average = 0.00;
float fps = 0;
int fpscount;
unsigned long lastfps = millis();
bool graph=true;

void scan_result();
void channel_dbm_graph();
void task_manager();
String encryption(int encryptionType);
String netDistance(int rssi);



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  tft.init();
  tft.setRotation(3);
  sprite.createSprite(160, 128);
}

void task_manager(){
  if(Serial.available()>0){
    char cmd=Serial.read();
    if(cmd=='S')networkCount = WiFi.scanNetworks();
    else if(cmd=='G')graph=!graph;
  }
  if(graph)channel_dbm_graph();
  if(!graph)scan_result();
  
}

void loop() {
  task_manager();
  
  
}

String netDistance(int rssi){
  return (String)((int)pow(10, (27.55 - (20 * log10(2400)) - rssi) / 20.0));
}


String encryption(int encryptionType){
switch (encryptionType) {
        case WIFI_AUTH_WEP:
          return "wep";
          break;
        case WIFI_AUTH_WPA_PSK:
          return "w+p";
          break;
        case WIFI_AUTH_WPA2_PSK:
          return "2+p";
          break;
        case WIFI_AUTH_OPEN:
          return "opn";
          break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
          return "Ent";
          break;
        case WIFI_AUTH_WPA_WPA2_PSK:
          return "w2p";
          break;
        default:
          return "n|a";
          break;
      }

}


void scan_result(){
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextColor(color1, 0xf80f);
  sprite.drawString("dbm dist ch  enc    ssid  ", 0, 0);
  sprite.setTextColor(TFT_GREEN, TFT_BLACK);
  for(int i=0;i<networkCount;i++){
  sprite.drawString((String)WiFi.RSSI(i), 0, 10+i*10);
  sprite.drawString(netDistance(WiFi.RSSI(i)),28, 10+i*10);
  sprite.drawString((String)WiFi.channel(i),55, 10+i*10);
  sprite.drawString(encryption(WiFi.encryptionType(i)),80, 10+i*10);
  sprite.drawString((String)(WiFi.SSID(i)),110, 10+i*10); 
    
  }
  sprite.pushSprite(0, 0);
}
void channel_dbm_graph(){
  sprite.fillSprite(color2);
  sprite.fillRoundRect(10, 10, 140, 105, 5, color1);
  sprite.setTextColor(color1, 0xf80f);
  sprite.drawString("Wifi Analyzer", 45, 0, 2);
  sprite.setTextColor(color1, color2);
  sprite.drawLine(20, 20, 20, 90, TFT_BLACK);
  sprite.drawLine(20, 90, 140, 90, TFT_BLACK);
  for (int y = 1; y < 8; y++) {
    for (int x = 1; x < 120; x = x + 5) {
      sprite.drawPixel(20 + x, 10 + 10 * y, color3);
    }
  }
  sprite.setTextColor(TFT_BLACK, color1);
  for (int y = 1; y < 8; y++) sprite.drawString(dbm[y - 1], 10, 10 + 10 * y);
  for (int x = 0; x < 10; x++) sprite.drawString(channel[x], 25 + (x * 10), 92);
  sprite.drawString(channel[10], 130, 92);
  sprite.setTextColor(TFT_WHITE, TFT_BLUE);
  sprite.drawString("Found:"+(String)networkCount, 15, 102);
  sprite.setCursor(0, 0);
  sprite.printf("%.0fFPS", fps);
  fpscount++;
  unsigned long fpsdelay = millis() - lastfps;
  if (fpsdelay >= 1000) {
    fps = float(fpscount * 1000) / float(fpsdelay);
    lastfps = millis();
    fpscount = 0;
  }
  sprite.pushSprite(0, 0);

}
