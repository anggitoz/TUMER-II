#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char auth[] = "hEtb5p_xHOl3Ih_Nn0mNN0ETAC5t-IlQ";
char ssid[] = "Tenda_218108"; //Enter your WIFI Name
char pass[] = "12345678";     //Enter your WIFI Password

#define DHTPIN    D3
#define DHTTYPE   DHT11    
#define TRIGGER   D5
#define ECHO      D6
#define LED       2
#define SOLEPIN   D7

const int AirValue      = 790;  
const int WaterValue    = 390;
const int SensorPin     = A0;
int soilMoistureValue   = 0;
int soilmoisturepercent = 0;
int waterLevel;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void sendSensor()
{
  long duration;
  float humidity = dht.readHumidity();
  float airTemp  = dht.readTemperature();

  soilMoistureValue = analogRead(SensorPin);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  
  if (isnan(humidity) || isnan(airTemp)) {
    lcd.setCursor(0,0);
    lcd.println("DHT11 terputus");
  }
  if (isnan(waterLevel)) {
    lcd.setCursor(0,1);
    lcd.println("HC-SR04 terputus");
  }
  lcd.print("T: ");
  lcd.print(dht.readTemperature(), 0);
  lcd.print(" C | H: ");
  lcd.print(dht.readHumidity(), 0);
  lcd.print("%");
  lcd.blink();
  lcd.setCursor(0, 1);
  lcd.print("s: ");
  //
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(12);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  waterLevel = (duration / 2) / 29.1;
  //
  lcd.print(waterLevel);
  lcd.print(" cm| M: ");
  lcd.print(soilmoisturepercent);
  lcd.print("%");
  delay(1000);
  lcd.clear();
  
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V4, LED);
  Blynk.virtualWrite(V5, humidity);  
  Blynk.virtualWrite(V6, airTemp);  
  Blynk.virtualWrite(V7, waterLevel);
  Blynk.virtualWrite(V8, soilmoisturepercent);
}

void Watering()
{
  if(waterLevel >= 100 && dht.readTemperature() >= 25) {
    digitalWrite(SOLEPIN, HIGH);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(2000);
  }
  else{
    digitalWrite(SOLEPIN, LOW);
    digitalWrite(LED, HIGH);
    delay(100); 
    digitalWrite(LED, LOW);
    delay(100);
  }
}

void setup()
{
  Serial.begin(9600); // See the connection status in Serial Monitor
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(SOLEPIN, OUTPUT);
  pinMode(LED, OUTPUT);
  Blynk.begin(auth, ssid, pass);

  dht.begin();
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Memuat...");
  lcd.blink();
  delay(3000);
  lcd.clear();
  lcd.print("-WEATHERSTATION-");
  lcd.setCursor(0,1);
  lcd.print(" Tumer Kel. 29");
  delay(5000);
  lcd.clear();

  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, Watering);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates BlynkTimer
}
