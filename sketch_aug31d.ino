#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TM1637Display.h>
#include <EEPROM.h>

// Definisikan pin untuk display TM1637
#define CLK D1 // GPIO5
#define DIO D2 // GPIO4

// Konfigurasi Access Point (AP) NodeMCU
const char* ap_ssid = "JamDigitalAP";
const char* ap_password = "BUMIKITA";

// Inisialisasi display TM1637
TM1637Display display(CLK, DIO);

// Inisialisasi web server di port 80
ESP8266WebServer server(80);

// Variabel untuk menyimpan nilai kecerahan
int brightness = 0x0a;

// Struktur data untuk disimpan di EEPROM
struct ClockData {
  bool wifiEnabled;
  long lastMillis;
  int jam;
  int menit;
  int tanggal;
  int bulan;
  int tahun;
};

// Variabel untuk menyimpan data waktu
ClockData clockData;

// Fungsi untuk membuat dan mengirim halaman web
void handleRoot() {
  String html = "<html><head><style>body{font-family: Arial, sans-serif; text-align: center;} input[type='submit']{padding: 10px 20px; font-size: 16px;}</style></head><body>";
  html += "<h1>Atur Jam & Tanggal</h1>";
  
  // Form untuk sinkronisasi waktu dan tanggal dengan HP
  html += "<form action='/set' method='get'>";
  html += "<input type='hidden' name='h' id='input_h'>";
  html += "<input type='hidden' name='m' id='input_m'>";
  html += "<input type='hidden' name='d' id='input_d'>";
  html += "<input type='hidden' name='mo' id='input_mo'>";
  html += "<input type='hidden' name='y' id='input_y'>";
  html += "<input type='submit' value='Sinkronkan dengan Waktu & Tanggal HP'>";
  html += "</form>";
  html += "<br>";
  
  // Form untuk pengaturan kecerahan
  html += "<form action='/set_brightness' method='get'>";
  html += "Kecerahan (0-7): <input type='number' name='b' min='0' max='7' value='" + String(brightness & 0x07) + "'><br>";
  html += "<input type='submit' value='Atur Kecerahan'>";
  html += "</form>";
  html += "<br>";

  // Tombol untuk mematikan WiFi
  html += "<form action='/turn_off_wifi' method='get'>";
  html += "<input type='submit' value='Matikan WiFi & Masuk Mode Hemat Daya'>";
  html += "</form>";
  html += "<br>";

  html += "<p>Waktu saat ini: " + String(clockData.jam) + ":" + String(clockData.menit) + "</p>";
  html += "<p>Tanggal saat ini: " + String(clockData.tanggal) + "/" + String(clockData.bulan) + "/" + String(clockData.tahun) + "</p>";

  // JavaScript untuk mengambil waktu dari browser
  html += "<script>";
  html += "function syncTime() {";
  html += "  var date = new Date();";
  html += "  var hours = date.getHours();";
  html += "  var minutes = date.getMinutes();";
  html += "  var day = date.getDate();";
  html += "  var month = date.getMonth() + 1;"; // getMonth() is 0-indexed
  html += "  var year = date.getFullYear();";
  html += "  document.getElementById('input_h').value = hours;";
  html += "  document.getElementById('input_m').value = minutes;";
  html += "  document.getElementById('input_d').value = day;";
  html += "  document.getElementById('input_mo').value = month;";
  html += "  document.getElementById('input_y').value = year;";
  html += "}";
  html += "document.querySelector('form').addEventListener('submit', syncTime);";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Handler untuk mematikan WiFi
void handleTurnOffWifi() {
  clockData.wifiEnabled = false;
  EEPROM.put(0, clockData);
  EEPROM.commit();
  server.send(200, "text/plain", "WiFi dimatikan, jam akan restart untuk masuk mode hemat daya. Silakan putuskan koneksi Anda.");
  delay(100);
  ESP.restart();
}

// Handler untuk mengatur waktu dan tanggal
void handleSet() {
  if (server.hasArg("h") && server.hasArg("m") && server.hasArg("d") && server.hasArg("mo") && server.hasArg("y")) {
    clockData.jam = server.arg("h").toInt();
    clockData.menit = server.arg("m").toInt();
    clockData.tanggal = server.arg("d").toInt();
    clockData.bulan = server.arg("mo").toInt();
    clockData.tahun = server.arg("y").toInt();
    clockData.lastMillis = millis();
    EEPROM.put(0, clockData);
    EEPROM.commit();
    server.send(200, "text/plain", "Waktu dan tanggal berhasil diatur!");
  } else {
    server.send(400, "text/plain", "Parameter tidak lengkap!");
  }
}

// Handler untuk mengatur kecerahan
void handleSetBrightness() {
  if (server.hasArg("b")) {
    int newBrightness = server.arg("b").toInt();
    brightness = 0x08 | (newBrightness & 0x07);
    display.setBrightness(brightness);
    server.send(200, "text/plain", "Kecerahan berhasil diatur!");
  } else {
    server.send(400, "text/plain", "Parameter tidak lengkap!");
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(sizeof(ClockData));
  EEPROM.get(0, clockData);

  // Jika ini adalah boot pertama (EEPROM kosong), aktifkan WiFi
  if (EEPROM.read(0) != 0 && EEPROM.read(0) != 1) {
    clockData.wifiEnabled = true;
    EEPROM.put(0, clockData);
    EEPROM.commit();
  }

  display.setBrightness(brightness);
  display.clear();
  
  if (clockData.wifiEnabled) {
    Serial.print("Menyiapkan Access Point...");
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(IP);

    server.on("/", handleRoot);
    server.on("/set", handleSet);
    server.on("/set_brightness", handleSetBrightness);
    server.on("/turn_off_wifi", handleTurnOffWifi);

    server.begin();
    Serial.println("Server berjalan dalam mode aktif.");
  } else {
    // Mode Deep Sleep
    WiFi.mode(WIFI_OFF);
    
    // Perbarui waktu dengan menaikkan 1 menit
    clockData.menit++;
    if (clockData.menit >= 60) {
      clockData.menit = 0;
      clockData.jam++;
      if (clockData.jam >= 24) {
        clockData.jam = 0;
        // Logika tanggal dan bulan dihilangkan untuk penyederhanaan
      }
    }
    EEPROM.put(0, clockData);
    EEPROM.commit();

    Serial.print("Waktu saat ini: ");
    Serial.print(clockData.jam);
    Serial.print(":");
    Serial.println(clockData.menit);
    
    // Tampilkan jam
    int timeDisplay = clockData.jam * 100 + clockData.menit;
    int colon = 0x40; // Nyalakan kolon
    display.showNumberDecEx(timeDisplay, colon, true, 4, 0); 
    
    Serial.println("Tidur selama 60 detik.");

    // Masuk ke mode Deep Sleep selama 60 detik
    ESP.deepSleep(60 * 1000000);
  }
}

void loop() {
  if (clockData.wifiEnabled) {
    server.handleClient();

    // Logika jam dan tanggal
    if (millis() - clockData.lastMillis >= 60000) {
      clockData.lastMillis = millis();
      clockData.menit++;
      if (clockData.menit >= 60) {
        clockData.menit = 0;
        clockData.jam++;
        if (clockData.jam >= 24) {
          clockData.jam = 0;
          // Logika tanggal dan bulan dihilangkan untuk penyederhanaan
        }
      }
      EEPROM.put(0, clockData);
      EEPROM.commit();
    }
  
    // Tampilkan jam saja secara terus-menerus
    int timeDisplay = clockData.jam * 100 + clockData.menit;
    int colon = (millis() % 1000 < 500) ? 0x40 : 0x00; // Tentukan status kolon berkedip
    display.showNumberDecEx(timeDisplay, colon, true, 4, 0); 
    
    delay(100);
  }
  // Jika WiFi mati, loop tidak melakukan apa-apa
}
