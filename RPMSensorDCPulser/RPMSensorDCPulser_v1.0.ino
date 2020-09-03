/*
  Arduino RPM Meter

  RPM meter ini dapat bekerja untuk mengukur kecepatan mesin sepeda motor yang sudah
  menggunakan pulser/pickup coil DC yang umumnya digunakan pada motor injeksi elektronik.
  
  Secara default, program ini akan membaca sinyal LOW dari pulser yang telah melewati
  optocoupler dan menghitung durasinya. Kemudian durasi tersebut dihitung periodenya untuk
  menentukan pembacaan RPM. Akan tetapi, karena setiap motor memiliki jumlah pulsa berbeda
  untuk tiap putarannya (tergantung jumlah tonjolan pada magnet rotor/fase alternator) maka nilai
  pulser_teeth harus disesuaikan.
  
  Rangkaian yang saya gunakan adalah rangkaian optocoupler PC817 yang ber input
  active HIGH dan ber output active LOW. Sangat disarankan menggunakan optocoupler agar sinyal
  yang masuk ke mikrokontroler bersih dan tidak merusak mikrokontroler.

  Harap membaca keterangan pada setiap baris kode untuk memahaminya.
  JANGAN SESEKALI menambahkan fungsi "delay" pada program ini karena akan menyebabkan
  program tidak berjalan!!! Gunakan millis() atau micros() untuk membuat pewaktuan.

  dibuat 2 Agustus 2019
  revisi 3 September 2020
  oleh Joshua Harbangan

  Silahkan dibagikan, dimodifikasi, dan dikembangkan untuk kemajuan bersama
*/

byte pin_pulser = 2; //pin yang dipakai untuk input sinyal pulser dari rangkaian (misal pin digital 2)
byte pulser_teeth = 6; //jumlah tonjolan pulser, atau merupakan jumlah sinyal pulser tiap satu putaran mesin
byte signal_pulser = 1; //variabel sinyal pulser yang terbaca (aktual)
byte last_signal_pulser = 1; //variabel sinyal pulser yang terakhir kali terbaca
byte marker = 0; //penanda state

unsigned long timeStart; //variabel waktu awal penghitungan durasi pulsa
unsigned long timeEnd; //variabel waktu akhir perhitungan durasi pulsa
unsigned long timeTotal; //variabel waktu total = waktu akhir - waktu awal
float revPerSecond; //variabel nilai RPS
float revPerMinute; //variabel nilai RPM

unsigned long timeNow = 0; //penanda waktu pengiriman data
byte interval_data = 200; //interval pengiriman data

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode (pin_pulser, INPUT_PULLUP); //inisialisasi pin pulser dalam untuk input active low

}

void loop() {
  // put your main code here, to run repeatedly:
  signal_pulser  = digitalRead(pin_pulser); //baca sinyal pulser

  if (marker == 0 && signal_pulser != last_signal_pulser){ //state 0 (perubahan sinyal pulser dari HIGH ke LOW)
      last_signal_pulser = signal_pulser; //menyamakan keadaan sinyal pulser terakhir dengan yang aktual
      if(signal_pulser == 0) { //saat sinyal pulser bernilai LOW maka lakukan perhitungan waktu dengan millis
       timeStart = micros(); //hitung waktu awal pulsa
       marker = 1; //penanda state 1
    }
  }


  if (marker == 1 && signal_pulser != last_signal_pulser){ //state 1 (perubahan sinyal pulser dari LOW ke HIGH)
      last_signal_pulser = signal_pulser; //menyamakan keadaan sinyal pulser terakhir dengan yang aktual
      if(signal_pulser == 1) { //saat sinyal pulser bernilai HIGH maka lakukan perhitungan waktu dengan millis
       timeEnd = micros(); //hitung waktu akhir pulsa
       marker = 0; //penanda state 0
       timeTotal = timeEnd - timeStart; //hitung waktu total durasi satu pulsa
    }
  }

    revPerSecond = (1000000/timeTotal)*(1/(2*pulser_teeth)); //RPS (s) = ( 1000000 x 1/waktu total(us) ) * 1/(2*jumlah tonjolan pulser)
    revPerMinute = revPerSecond*60; //RPM = 60 x RPS
 
//----routine untuk menampilkan pembacaan----//

if(millis()-timeNow >= interval_data){ //setiap interval tertentu kirimkan data
  timeNow = millis();
  Serial.println(revPerMinute); //lakukan print data serial, ganti di bagian ini untuk penggunaan dengan LCD atau display lainnya
}

}
