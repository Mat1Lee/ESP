///////Firebase & Wifi
#define FIREBASE_HOST "diennuoc-9bba6-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "IOWgKGGyWx2qcXEKhDAOW1GYBSMwOBCiMcdM5XhG"
#define WIFI_SSID "Manh Le"
#define WIFI_PASSWORD "manhthuong2022"
/////// PZEM
#define PZEM_RX_PIN D1
#define PZEM_TX_PIN D2
#define SIM_RX D5
#define SIM_TX D6
String SDT = "+84979259510";
///////LÆ°u LÆ°á»£ng
#define S401 digitalRead(D3)
/// Other
float luuLuong, theTich;
unsigned long xung, timer1, timer2;
float dienRead, dienSet, dienCal;
int modeDay = 0, modeMonth = 0, countDay =0;
float dienMonth[31], nuocMonth[31];
float totalDien, totalNuoc;

int SoDien_CB;
bool sts_getData = false;

uint8_t ngay = 0;
uint8_t thang = 0;
uint8_t gio = 0;
uint8_t phut = 0;
uint16_t nam = 0;
bool sts = false;
bool str = false;
static const char ntpServerName[] = "us.pool.ntp.org";
const int timeZone = 7;
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
unsigned int localPort = 8888;  // local port to listen for UDP packets

String parentPath = "/SetUp";
String childPath[2] = {"/HanMuc", "/Status"};
