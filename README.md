# ⏰ Jam Digital WiFi & Hemat Daya dengan NodeMCU ⏳

Proyek ini adalah implementasi jam digital menggunakan mikrokontroler **NodeMCU ESP8266** dan **display 7-segmen TM1637**. Keunikan proyek ini adalah kemampuannya untuk beroperasi dalam dua mode: mode konfigurasi WiFi dan mode hemat daya (*deep sleep*) untuk menghemat baterai.

---

## 💡 Fitur Utama

* **Jam Digital & Tanggal**: Menampilkan waktu (jam dan menit) yang presisi pada display 7-segmen.
* **Mode Konfigurasi WiFi**: Saat dinyalakan, ESP8266 akan berfungsi sebagai **Access Point (AP)**, memungkinkan Anda terhubung dari ponsel atau laptop.
* **Sinkronisasi Waktu Mudah**: Setelah terhubung ke AP, Anda dapat membuka halaman web lokal di browser HP Anda untuk **secara instan menyinkronkan waktu** jam dengan waktu perangkat Anda.
* **Pengaturan Kecerahan**: Halaman web menyediakan kontrol untuk mengatur kecerahan display TM1637.
* **Mode Hemat Daya (*Deep Sleep*)**: Setelah konfigurasi selesai, Anda dapat mematikan WiFi melalui halaman web. Jam akan me-restart, mematikan WiFi, dan masuk ke mode hemat daya, di mana ia akan "bangun" setiap 60 detik hanya untuk memperbarui dan menampilkan waktu, lalu tidur kembali.
* **Memori Non-Volatile (EEPROM)**: Waktu, tanggal, dan status WiFi disimpan di memori EEPROM, memastikan data tidak hilang saat daya mati atau saat perangkat tidur.

---

## ⚙️ Persiapan dan Komponen

### Komponen yang Dibutuhkan
1. **NodeMCU ESP8266**
2. **Display 7-segmen TM1637** (4 digit)
3. Kabel Jumper

### Instalasi dan Kebutuhan Software
1. **Arduino IDE**: Pastikan Anda telah menginstal [Arduino IDE](https://www.arduino.cc/en/software).
2. **Board ESP8266**: Tambahkan *board manager* ESP8266 ke Arduino IDE Anda. Caranya:
    * Buka `File > Preferences`.
    * Pada `Additional Boards Manager URLs`, masukkan: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
    * Buka `Tools > Board > Boards Manager` dan cari `esp8266` untuk menginstalnya.
3. **Perpustakaan (Library)**: Instal dua *library* berikut melalui `Tools > Manage Libraries...`:
    * `ESP8266WiFi` (Sudah termasuk dengan *board* ESP8266)
    * `ESP8266WebServer` (Sudah termasuk dengan *board* ESP8266)
    * `TM1637Display` (Cari di *library manager* dan instal, atau unduh dari [GitHub](https://github.com/avishay-il/TM1637)).
    * `EEPROM` (Sudah termasuk dengan Arduino IDE)

---

## 🔌 Rangkaian (Wiring)

Hubungkan display TM1637 ke NodeMCU sebagai berikut:

| TM1637 | NodeMCU ESP8266 |
| :----: | :-------------: |
| **VCC** | **3V3** |
| **GND** | **GND** |
| **CLK** | **D1** (GPIO5) |
| **DIO** | **D2** (GPIO4) |

---

## 🚀 Cara Menggunakan

### Mode Konfigurasi (WiFi Aktif)
1. *Upload* kode ke NodeMCU.
2. Setelah berhasil, NodeMCU akan membuat **Access Point** dengan nama `JamDigitalAP` dan *password* `BUMIKITA`.
3. Hubungkan ponsel atau laptop Anda ke AP tersebut.
4. Buka browser Anda dan navigasi ke alamat **`192.168.4.1`**.
5. Di halaman yang muncul, Anda akan melihat opsi untuk:
    * **Sinkronkan dengan Waktu & Tanggal HP**: Tekan tombol ini untuk mengatur jam dan tanggal berdasarkan perangkat Anda.
    * **Atur Kecerahan**: Gunakan *form* untuk mengubah kecerahan display.
    * **Matikan WiFi & Masuk Mode Hemat Daya**: Tekan ini setelah Anda selesai melakukan konfigurasi.

### Mode Hemat Daya (*Deep Sleep*)
1. Setelah menekan tombol **"Matikan WiFi"**, NodeMCU akan me-restart.
2. WiFi akan dimatikan, dan perangkat akan masuk ke mode hemat daya.
3. Display akan terus menampilkan waktu, dan ESP8266 akan bangun setiap 60 detik (1 menit) untuk memperbarui waktu dan kembali tidur.
4. Mode ini sangat efisien untuk penggunaan baterai jangka panjang.
5. Untuk mengaktifkan kembali mode konfigurasi, Anda perlu me-restart NodeMCU secara manual (cabut dan pasang kembali daya).

---

## 📄 Lisensi

Proyek ini dirilis di bawah **Lisensi MIT**. Anda bebas menggunakan, memodifikasi, dan menyebarkannya untuk tujuan apa pun.

---

## 🤝 Kontribusi

Proyek ini bersifat *open-source*. Jika Anda memiliki saran perbaikan, menemukan *bug*, atau ingin menambahkan fitur baru, silakan:
* Buka **Issues** di repositori GitHub ini.
* Buat **Pull Request** dengan perubahan Anda.

Mari kembangkan proyek ini bersama!
