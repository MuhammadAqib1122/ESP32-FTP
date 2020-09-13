#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include "ESP32_FTPClient_updated.h"

#define WIFI_SSID ""
#define WIFI_PASS ""

const int buf = 16000;
uint8_t buff1[buf];
int uploadbuf_len = buf;
char ftp_server[] = "";
char ftp_user[]   = "";
char ftp_pass[]   = "";
ESP32_FTPClient_updated ftp (ftp_server, ftp_user, ftp_pass, 5000, 2);

void setup()
{
  Serial.begin(115200);
  Serial.print("Setup");
  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  Serial.println("SD CARD INITIALIZED");

  WiFi.begin( WIFI_SSID, WIFI_PASS );

  Serial.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  File fl = SD_MMC.open("/audios");
  if (!fl) {
    Serial.println("Failed to open folder");
    return;
  }
  ftp.OpenConnection();
  ftp.MakeDir(fl.name());
  File audio_fl = fl.openNextFile();
  while (audio_fl)
  {
    String audio_flname = audio_fl.name();
    int audio_flsize = audio_fl.size();
    Serial.print("Audio uploading :");
    Serial.println(audio_flname);
    ftp.InitFile("Type A");
    ftp.NewFile(audio_flname.c_str());
    while (audio_flsize)
    {
      if (audio_flsize >= uploadbuf_len)
      {
        uploadbuf_len = buf;
        audio_fl.read(buff1, uploadbuf_len);
      }
      else
      {
        uploadbuf_len = audio_flsize;
        audio_fl.read(buff1, audio_flsize);
      }
      ftp.WriteData( buff1, uploadbuf_len);
      audio_flsize = audio_fl.available();
    }
    ftp.CloseFile();
    audio_fl = fl.openNextFile();
  }
  audio_fl.close();
  ftp.CloseConnection();
}

void loop()
{
  delay(1);
}
