#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "KhaiBao.h"

void ICACHE_RAM_ATTR DEM_XUNG();               // khai báo nguyên hàm mẫu
SoftwareSerial SIM(SIM_RX, SIM_TX);
#include "SIM800L.h"
FirebaseData fbdt;
FirebaseData stream;
SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);
WiFiUDP Udp;
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
void GetTime();

void setup() {
  Serial.begin(74880);
  pinMode(D3, INPUT);
  pinMode(D5, INPUT);
  pinMode(D6, OUTPUT);
  SIM.begin(9600);
  Connect();
  attachInterrupt(digitalPinToInterrupt(D3), DEM_XUNG, FALLING);
  timer1 = millis();
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  GetTime();
  pzem.resetEnergy();

}
void loop() {
  if (Firebase.ready())
  {
    while (str == false)
    {
      dienSet = pzem.power();
      delay(1000);
      Serial.println(dienSet);
      Serial.println("ok");
      str = true;
    }
    SendData();
    UpCurrentData();
  }

}
void UpCurrentData()
{
  if (sts_getData == true)
  {
    String Path = "Value";
    dienRead = pzem.power();
    dienCal = fabs(dienRead - dienSet)*10;
    Firebase.setFloat(fbdt, Path + "/Dien", dienCal) ?
    Serial.println("cập nhật số điện hiện tại xong") :
    Serial.println("Error");
    Firebase.setFloat(fbdt, Path + "/Nuoc", luuLuong) ?
    Serial.println("cập nhật số nước hiện tại xong") :
    Serial.println("Error");
    sts_getData = false;
    Firebase.setBool(fbdt, "SetUp/Status", sts_getData);
  }
}
void SendData() {
  if (millis() - timer1 > 9000) {
    if (modeDay == 0) {
      while (isnan(pzem.power())) {}
      dienRead = pzem.power();
    
      dienCal = fabs(dienRead - dienSet)*10;
      dienMonth[ngay] = dienCal;
      nuocMonth[ngay] = luuLuong;

      Serial.println("Nuoc: " + (String)luuLuong + "---Dien: " + (String)dienRead);
      if (dienMonth[ngay] > SoDien_CB)
      {
        GUISMS(SDT, "Canh bao su dung dien qua muc!");
      }
      String ngay_Set, thang_Set;
      if (ngay < 10) ngay_Set += '0';
      ngay_Set += (String)ngay;
      if (thang < 10) thang_Set += '0';
      thang_Set += (String)thang;
      String parentPath = "DienNuoc/" +(String)nam + '-' + thang_Set+ '-' +ngay_Set ;
      Firebase.setFloat(fbdt, parentPath + "/Dien", dienCal) ?
      Serial.println("cập nhật số điện xong") :
      Serial.println("Error");
      Firebase.setFloat(fbdt, parentPath + "/Nuoc", luuLuong) ?
      Serial.println("cập nhật số nước xong") :
      Serial.println("Error");
      dienSet = dienRead;
      xung = 0;
      modeDay = 1;
      SendMonth();
      DateCal();
      countDay ++;
      timer1 = millis();
      Serial.println(countDay);
    }
  }
  else {
    modeDay = 0;
    S401Cal();
  }
}
void SendMonth() {
  if (countDay == 30) {
    if (modeMonth == 0) {
      String parentPath = "Total/" + (String)nam +'-' + (String)thang ;
      for (int i = 1; i <= 30; i++) {
        totalDien += dienMonth[i];
        totalNuoc += nuocMonth[i];
      }
      Firebase.setFloat(fbdt, parentPath + "/Dien", totalDien)  ?
      Serial.println(" Update điện 1 tháng xong") :
      Serial.println("Error1");

      Firebase.setFloat(fbdt, parentPath + "/Nuoc", totalNuoc)  ?
      Serial.println(" Update nước 1 tháng xong") :
      Serial.println("Error2");
      modeMonth = 1;
    }
  }
  else {
    modeMonth = 0;
  }
}
void DateCal()
{
  ngay++;
  if (ngay > 30)
  {
    ngay = 1;
    
    thang ++;
  }
  if (thang > 12)
  {
    thang = 1;
    nam++;
  }if(countDay == 30){
    countDay=1;
  }
}
void GetTime()
{
  gio = hour();
  phut = minute();
  ngay = day();
  thang = month();
  nam = year();
  Serial.print(gio);
  Serial.print(":");
  Serial.print(phut);
  Serial.print("    ");
  Serial.print(ngay);
  Serial.print("---");
  Serial.print(thang);
  Serial.print("---");
  Serial.print(nam);

  Serial.println();
}
void S401Cal() {
  luuLuong = 1000 * xung / 98.0 / 60.0;  // CT tính lưu lượng - đơn vị mL/s

  // Serial.println((String)luuLuong);
}
void ICACHE_RAM_ATTR DEM_XUNG() {
  xung ++;
  //  Serial.println(xung);
}
void Connect() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);      // khởi chạy chế độ STATION
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {     // đợi trong tgian kết nối
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected: ");
  Serial.print("SSID: "); Serial.println(WIFI_SSID);
  Serial.print("PASS: "); Serial.println(WIFI_PASSWORD);
  Serial.print("IP  : "); Serial.println(WiFi.localIP());
  // startSIM();
  initSIM();
  GUISMS(SDT, "Khoi dong xong!");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
#if defined(ESP8266)
  stream.setBSSLBufferSize(2048 /* Rx in bytes, 512 - 16384 */, 512 /* Tx in bytes, 512 - 16384 */);
#endif
  if (!Firebase.beginMultiPathStream(stream, parentPath))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);
  Serial.println("ok");
}
time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
