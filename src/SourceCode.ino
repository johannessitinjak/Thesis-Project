#include <math.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd (0x3F, 16, 2);
#define analogInPin A0  //sambungkan kabel hitam (output) ke pin A0
#define sensor A1
#define wet 210
#define dry 510
int sensorValue = 0;        //ADC value from sensor
float inputPh = 0.0;        //pH value after conversion

void display_freeram() {
  Serial.print(F("- SRAM Digunakan: "));
  Serial.println(2048 - freeRam());
}

int freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0  
  ? (int)&__heap_start : (int) __brkval);  
}

float standarDeviasi(float latih[], float mean){
  float dev = 0;
  int panjangArray = 10;
  for(int i = 0; i< panjangArray; i++){
    dev += pow(latih[i]-mean,2);
  }
  dev /= panjangArray -1;
  return sqrt(dev);
}
float klasifikasiGausian(float inputan, float mean, float deviasi) {
  float hasil = 1 / (deviasi * sqrt(2 * PI));
  float hasil1 = sq(inputan - mean);
  hasil1 = (hasil1 / (2 * sq(deviasi))) * -1;
  hasil1 = pow(exp(1), hasil1);
  hasil = hasil * hasil1;
  return hasil*100;
}

float klasifikasi(float nilai1, float nilai2, float peluang){
  float hasil = nilai1*nilai2*peluang;
  return hasil;
}

const size_t jmlh = 30;
int jmlhBaikPh = 0;
int jmlhSedangPh = 0;
int jmlhBurukPh = 0;
int jmlhBaikKelembapan = 0;
int jmlhSedangKelembapan = 0;
int jmlhBurukKelembapan = 0;
float banyakBaik= 0;
float banyakSedang= 0;
float banyakBuruk= 0;
float latihBaikPh[10];
float latihSedangPh[10];
float latihBurukPh[10];
float latihBaikKelembapan[10];
float latihSedangKelembapan[10];
float latihBurukKelembapan[10];
float latih[jmlh][3] ={
  {80, 6, 0},
  {61, 7, 0},
  {65, 7, 0},
  {74, 7, 0},
  {70, 7, 0},
  {67, 7, 0},
  {77, 6, 0},
  {73, 7, 0},
  {69, 6, 0},
  {69, 7, 0},
  {65, 6, 1},
  {82, 7, 1},
  {75, 8, 1},
  {68, 5, 1},
  {78, 5, 1},
  {63, 6, 1},
  {53, 6, 1},
  {66, 6, 1},
  {65, 5, 1},
  {53, 5, 1},
  {64, 4, 2},
  {45, 6, 2},
  {70, 4, 2},
  {65, 4, 2},
  {47, 6, 2},
  {44, 8, 2},
  {80, 3, 2},
  {83, 4, 2},
  {40, 8, 2},
  {48, 7, 2}
};


void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  int indexArrayBaik = 0;
  int indexArraySedang = 0;
  int indexArrayBuruk = 0;
    for(int i=0; i<jmlh; i++){
      if(latih[i][2] == 0){
        banyakBaik += 1;
        jmlhBaikPh = jmlhBaikPh + latih[i][1];
        jmlhBaikKelembapan = jmlhBaikKelembapan + latih[i][0];
        latihBaikPh[indexArrayBaik] = latih[i][1];
        latihBaikKelembapan[indexArrayBaik] = latih[i][0];
        indexArrayBaik++;
      }else if(latih[i][2] == 1){
        banyakSedang += 1;
        jmlhSedangPh = jmlhSedangPh + latih[i][1];
        jmlhSedangKelembapan = jmlhSedangKelembapan + latih[i][0];
        latihSedangPh[indexArraySedang] = latih[i][1];
        latihSedangKelembapan[indexArraySedang] = latih[i][0];
        indexArraySedang++;
      }else if(latih[i][2] == 2){
        banyakBuruk += 1;
        jmlhBurukPh = jmlhBurukPh + latih[i][1];
        jmlhBurukKelembapan = jmlhBurukKelembapan + latih[i][0];
        latihBurukPh[indexArrayBuruk] = latih[i][1];
        latihBurukKelembapan[indexArrayBuruk] = latih[i][0];
        indexArrayBuruk++;
      }
    }
}

void loop(){
  lcd.clear();
  unsigned long startTimeTraining = millis();

  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  inputPh = (-0.0693*sensorValue)+7.3855;

  //sensor kelembapan
  int value = analogRead(sensor);
  int inputKelembapan = map(value, wet, dry, 100, 0);

  //mencari peluang dari setiap kelas
  float peluangBaik = banyakBaik/jmlh;
  float peluangSedang = banyakSedang/jmlh;
  float peluangBuruk = banyakBuruk/jmlh;

  //mencari Mean
  float meanBaikPh = jmlhBaikPh/banyakBaik;
  float meanBaikKelembapan = jmlhBaikKelembapan/banyakBaik;
  float meanSedangPh = jmlhSedangPh/banyakSedang;
  float meanSedangKelembapan = jmlhSedangKelembapan/banyakSedang;
  float meanBurukPh = jmlhBurukPh/banyakBuruk;
  float meanBurukKelembapan = jmlhBurukKelembapan/banyakBuruk;

  //mencari standar deviasi
  float stdDevBaikPh = standarDeviasi(latihBaikPh, meanBaikPh);
  float stdDevSedangPh = standarDeviasi(latihSedangPh, meanSedangPh);
  float stdDevBurukPh = standarDeviasi(latihBurukPh, meanBurukPh);
  float stdDevBaikKelembapan = standarDeviasi(latihBaikKelembapan, meanBaikKelembapan);
  float stdDevSedangKelembapan = standarDeviasi(latihSedangKelembapan, meanSedangKelembapan);
  float stdDevBurukKelembapan = standarDeviasi(latihBurukKelembapan, meanBurukKelembapan);
  unsigned long endTimeTraining = millis();

  unsigned long startTimeKlasifikasi = millis();
  //memanggil rumus gausian
  double kelasPhBaik = klasifikasiGausian(inputPh, meanBaikPh, stdDevBaikPh);
  double kelasPhSedang = klasifikasiGausian(inputPh, meanSedangPh, stdDevSedangPh);
  double kelasPhBuruk = klasifikasiGausian(inputPh, meanBurukPh, stdDevBurukPh);
  double kelasKelembapanBaik = klasifikasiGausian(inputKelembapan, meanBaikKelembapan, stdDevBaikKelembapan);
  double kelasKelembapanSedang = klasifikasiGausian(inputKelembapan, meanSedangKelembapan, stdDevSedangKelembapan);
  double kelasKelembapanBuruk = klasifikasiGausian(inputKelembapan, meanBurukKelembapan, stdDevBurukKelembapan);

  //klasifikasi
  double kelasBaik = klasifikasi(kelasPhBaik, kelasKelembapanBaik, peluangBaik);
  double kelasSedang = klasifikasi(kelasPhSedang, kelasKelembapanSedang, peluangSedang);
  double kelasBuruk = klasifikasi(kelasPhBuruk, kelasKelembapanBuruk, peluangBuruk);
  unsigned long endTimeKlasifikasi = millis();

  lcd.setCursor(0, 0);
  lcd.print("Kelembapan: ");
  lcd.setCursor(12, 0);
  lcd.print(inputKelembapan);
  lcd.print("%");
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print("pH Tanah: ");
  lcd.setCursor(11, 1);
  lcd.print(inputPh);

  delay(2000);
 
  if(kelasBaik > kelasSedang && kelasBaik > kelasBuruk){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kualitas: ");
    lcd.setCursor(0, 1);
    display_freeram();
    lcd.print("Baik");
    lcd.setCursor(0, 1);
    // lcd.print("Waktu : ");
    // lcd.setCursor(9, 1);
    // lcd.print(waktuKomputasiKlasifikasi);
    // lcd.setCursor(11, 1);
    // lcd.print("ms");
    // Serial.println("Baik");
  }else if(kelasBaik < kelasSedang && kelasSedang > kelasBuruk){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kualitas: ");
    lcd.setCursor(0, 1);
    display_freeram();  
    lcd.print("Sedang");
    lcd.setCursor(0, 1);
    // lcd.print("Waktu : ");
    // lcd.setCursor(9, 1);
    // lcd.print(waktuKomputasiKlasifikasi);
    // lcd.setCursor(11, 1);
    // lcd.print("ms");
    // Serial.println("Sedang");
  }else if(kelasBuruk > kelasBaik && kelasSedang < kelasBuruk){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kualitas: ");
    lcd.setCursor(0, 1);
    lcd.print("Buruk");
    display_freeram();
    lcd.setCursor(0, 1);
    // lcd.print("Waktu : ");
    // lcd.setCursor(9, 1);
    // lcd.print(waktuKomputasiKlasifikasi);
    // lcd.setCursor(11, 1);
    // lcd.print("ms");
    // Serial.println("Buruk");
  };
  unsigned long waktuKomputasiTraining = endTimeTraining - startTimeTraining;
  unsigned long waktuKomputasiKlasifikasi = endTimeKlasifikasi - startTimeKlasifikasi;
  // Serial.println(inputPh);
  Serial.print("Waktu Komputasi Training : ");
  Serial.println(waktuKomputasiTraining);
  Serial.print("Waktu Komputasi Klasifikasi : ");
  Serial.println(waktuKomputasiKlasifikasi);

  delay(2000);
}