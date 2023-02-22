/*************************************************************

  General Setup

 *************************************************************/

// This script is cycles the 8 LEDs and takes a reading using each LED being on. The results are display on the G9 Display.

#include "tlc59208.h" // Small homemade definition library for TLC59208
#include <Wire.h>     // The I2C library
#include <Arduino.h>  // Some Arduino thing
#include <unity.h>    // Unit testing for C
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <math.h>
#include <Adafruit_AS7341.h>
#include <model.h>


#define NUMPIXELS 1

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
NAU7802 nau;
TLC59208 tlc;
Adafruit_AS7341 as7341;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

static const int N_OUTPUTS = 8; // Number of LEDs to control
static const int RESET = 2;     // //Define reset pin for TLC59208
static const int ADDR = 0x20;   // The TLC59208 I2C address.



/*************************************************************

  Setup of blynk

 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLS715zmNE"
#define BLYNK_DEVICE_NAME "PlasticScanner Control"
#define BLYNK_AUTH_TOKEN            "sXlf6-oNAP7nxzT5Flql3dxhLz5wyEno"


BlynkTimer timer;

char auth[] = BLYNK_AUTH_TOKEN;
// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

int led = LED_BUILTIN;


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "bifrost2022";
char pass[] = "SorenOgMads"; 



/*************************************************************

  Firebase Setup
  
 *************************************************************/


// Insert Firebase project API Key
#define API_KEY "AIzaSyCi2agiwofeoXMfrJCiGY666EdQj6JeHQo"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "glavindmarkus@gmail.com"
#define USER_PASSWORD "PlasticScanner"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://plasticscanner-cc80a-default-rtdb.europe-west1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;


// Database child nodes
String reading855path   = "/reading855" ;
String reading940path   = "/reading940" ;
String reading1050path  = "/reading1050";
String reading1300path  = "/reading1300";
String reading1460path  = "/reading1460";
String reading1650path  = "/reading1650";
String reading1720path  = "/reading1720";


String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";





/*************************************************************

  Default values
  
 *************************************************************/

int LogMode   = 0;
int TypePET   = 0;
int TypeHDPE  = 0;
int TypePVC   = 0;
int TypeLDPE  = 0;
int TypePP    = 0;
int TypePS    = 0;

String CurrentType = "Unknown";

// Reading default value - they all should have a database node!
String reading855Value  = "0";  
String reading940Value  = "0";  
String reading1050Value = "0"; 
String reading1300Value = "0"; 
String reading1460Value = "0"; 
String reading1650Value = "0"; 
String reading1720Value = "0"; 


/*************************************************************

  Blynk updaters
  
 *************************************************************/

// This function is controlling the Scan mode
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  LogMode = param.asInt();
  Serial.println(LogMode); 

  if (LogMode == 0) {
    digitalWrite(led, LOW);
    Serial.println("LogMode was 0!"); 
  }
  if (LogMode == 1) {
    digitalWrite(led, HIGH);
    Serial.println("LogMode was 1!"); 
  }
  // Update state
  Blynk.virtualWrite(V1, LogMode);
}

// This function is Setting Scan type to PET
BLYNK_WRITE(V7)
{
  // Set incoming value from pin V0 to a variable
  TypePET = param.asInt();
  Serial.println("PET mode set to: "); 
  Serial.println(TypePET); 

  // Update state
  Blynk.virtualWrite(V12, TypePET);
}
// This function is Setting Scan type to HDPE
BLYNK_WRITE(V4)
{
  // Set incoming value from pin V0 to a variable
  TypeHDPE = param.asInt();
  Serial.println("HDPE mode set to: "); 
  Serial.println(TypeHDPE); 

  // Update state
  Blynk.virtualWrite(V13, TypeHDPE);
}
// This function is Setting Scan type to PVC
BLYNK_WRITE(V6)
{
  // Set incoming value from pin V0 to a variable
  TypePVC = param.asInt();
  Serial.println("PVC mode set to: "); 
  Serial.println(TypePVC); 

  // Update state
  Blynk.virtualWrite(V14, TypePVC);
}

// This function is Setting Scan type to LDPE
BLYNK_WRITE(V8)
{
  // Set incoming value from pin V0 to a variable
  TypeLDPE = param.asInt();
  Serial.println("LDPE mode set to: "); 
  Serial.println(TypeLDPE); 

  // Update state
  Blynk.virtualWrite(V15, TypeLDPE);
}


// This function is Setting Scan type to PP
BLYNK_WRITE(V5)
{
  // Set incoming value from pin V0 to a variable
  TypePP = param.asInt();
  Serial.println("PP mode set to: "); 
  Serial.println(TypePP); 

  // Update state
  Blynk.virtualWrite(V16, TypePP);
}

// This function is Setting Scan type to PS
BLYNK_WRITE(V9)
{
  // Set incoming value from pin V0 to a variable
  TypePS = param.asInt();
  Serial.println("PS mode set to: "); 
  Serial.println(TypePS); 

  // Update state
  Blynk.virtualWrite(V17, TypePS);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}




/*************************************************************

  Setup void
  
 *************************************************************/


void setup()
{
  // Debug console
  delay(2000);
  Serial.begin(115200);
  Serial.println("Serial started");

  Blynk.begin(auth, ssid, pass);
  Serial.println("Blynk begun");

                                                
  timer.setInterval(1000L, myTimerEvent); // Setup a function to be called every second

  initFirebase();    
  pinMode(led, OUTPUT);           // set LED to be an output pin
  Wire.begin();                   // Define the I2C library for use.
  tlc.begin();                    // Define the TLC library for use.
  NeopixelSetup();
  tftSetup();
  ADCSetup();
  ColorSensorSetup();

}



/*************************************************************

  Loop
  
 *************************************************************/


void loop(){

  // Get information from Blynk
  Blynk.run();
  timer.run();

  // Set polymer type from Blynk info
  if (TypePET == 1){
    CurrentType = "PET";
    Serial.println(CurrentType);
  }
  if (TypeHDPE == 1){
    CurrentType = "HDPE";
    Serial.println(CurrentType);
  }
  if (TypePVC == 1){
    CurrentType = "PVC";
    Serial.println(CurrentType);
  }
  if (TypeLDPE == 1){
    CurrentType = "LDPE";
    Serial.println(CurrentType);
  }
  if (TypePP == 1){
    CurrentType = "PP";
    Serial.println(CurrentType);
  }
  if (TypePS == 1){
    CurrentType = "PS";
    Serial.println(CurrentType);
  }


  if digitalRead(5) == LOW {
     // Execute a scan and display the results.
    UNITY_BEGIN();
    RUN_TEST(runScan);
    UNITY_END();
  }

}




SendToFirebase() {
    // Send new readings to database
  if (Firebase.ready()) {

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(reading855path.c_str(),  String(reading855Value));
    json.set(reading940path.c_str(),  String(reading940Value));
    json.set(reading1050path.c_str(), String(reading1050Value));
    json.set(reading1300path.c_str(), String(reading1300Value));
    json.set(reading1460path.c_str(), String(reading1460Value));
    json.set(reading1650path.c_str(), String(reading1650Value));
    json.set(reading1720path.c_str(), String(reading1720Value));


    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
  else{
    Serial.println("Firebase failed");
  }
}


/*************************************************************

  LED Driver - TLC59208
  
 *************************************************************/
initFirebase() {
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

}

/*************************************************************

  LED Driver - TLC59208
  
 *************************************************************/

void TLC59208::begin()
{
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, HIGH);

    reset();

    // Configuration, write to all registers
    Wire.beginTransmission(ADDR);
    Wire.write(0x80); // Control: auto-increment + register address
    Wire.write(0x81); // 00h: MODE1   Auto-increment, disable sleep mode, allcall address enabled
    Wire.write(0x00); // 01h: MODE2
    Wire.write(0x00); // 02h: PWM0
    Wire.write(0x00); // 03h: PWM1
    Wire.write(0x00); // 04h: PWM2
    Wire.write(0x00); // 05h: PWM3
    Wire.write(0x00); // 06h: PWM4
    Wire.write(0x00); // 07h: PWM5
    Wire.write(0x00); // 08h: PWM6
    Wire.write(0x00); // 09h: PWM7
    Wire.write(0x00); // 0Ah: GRPPWM
    Wire.write(0x00); // 0Bh: GRPFREQ
    Wire.write(0xAA); // 0Ch: LEDOUT0   LEDs controlled via PWM registers
    Wire.write(0xAA); // 0Dh: LEDOUT1   LEDs controlled via PWM registers
    Wire.write(0x00); // 0Eh: SUBADR1
    Wire.write(0x00); // 0Fh: SUBADR2
    Wire.write(0x00); // 10h: SUBADR3
    Wire.write(0x00); // 11h: ALLCALLADR
    bool ret = Wire.endTransmission();

    assert(ret == 0); // Fail if data not ACKed
}

void TLC59208::on(int output)
{
    setBrightness(output, 100);
}

void TLC59208::off(int output)
{
    setBrightness(output, 0);
}

/*************************************************************

  ADC - Nau7802
  
 *************************************************************/

void ADCSetup() {
    // NAU7802 SETUP //
    if (nau.begin() == false)
    {
        Serial.println("NAU7802 not detected.");
    }
    else {
      Serial.println("NAU7802 detected!");
    }
    
    nau.setGain(NAU7802_GAIN_1);
    // float theSampleRate = nau.setSampleRate;
    // Serial.println(theSampleRate);
}


/*************************************************************

  Color Sensor - AS7341
  
 *************************************************************/

void ColorSensorSetup() {
    // AS7341
    as7341.setATIME(100);
    as7341.setASTEP(999);
    as7341.setGain(AS7341_GAIN_256X);
}


/*************************************************************

  NeoPixel - Onboard 
  
 *************************************************************/

 void NeopixelSetup() {
  // NEOPIXEL SETUP
  pixels.begin();             // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(80);
  pixels.fill(0xFF0000);      // Set color of NEO Pixel to RED.
  pixels.show();              // Indicate the device is now running code from setup.
 }

/*************************************************************

  Screen control
  
 *************************************************************/

void tftSetup() {
  // TFT SETUP
  tft.init();
  tft.fillScreen(TFT_DARKGREEN);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("PlasticScanner", 120, 120, 4);
  tft.drawCentreString("Ready", 120, 140, 4);
  tft.setTextDatum(MC_DATUM);

  pixels.fill(0xE6CC00); // Set color of NEO Pixel to YELLOW.
  pixels.show();         // Indicate the device is now done with setup.
}

static void reset()
{
  digitalWrite(RESET, LOW);
  delayMicroseconds(1);
  digitalWrite(RESET, HIGH);
  delayMicroseconds(1);
}

static void setBrightness(int output, int percents)
{
  assert((output >= 0) && (output < N_OUTPUTS));
  assert((percents >= 0) && (percents <= 100));

  uint8_t reg = 0x02 + output;
  uint8_t pwm = (percents * 255) / 100;

  Wire.beginTransmission(ADDR);
  Wire.write(reg);
  Wire.write(pwm);
  Wire.endTransmission();
}


void ReadingDone() {
  tft.fillScreen(TFT_DARKGREEN);
  tft.drawCentreString("Reading done", 120, 100, 4);
  // tft.drawCentreString("...", 120, 140, 4);

}

void ShowProgress(String progressOutput) {
  tft.fillRect(0, 140, 240, 180, TFT_DARKGREEN); // clear a region of the display with the background color
  tft.drawCentreString(progressOutput, 120, 140, 4);

}

void scanningScreen() {
  pixels.fill(0x02198B);                          // Set color of NEO Pixel to BLUE.
  pixels.show();                                  // Indicate the device is now running a scan.

  tft.fillScreen(TFT_DARKGREEN);
  tft.drawCentreString("Scanning", 120, 120, 4);
  Serial.println("Starting scan.");
}

void ReadyToScan() {
  tft.fillScreen(TFT_DARKGREEN);
  tft.drawCentreString("Ready to scan", 120, 110, 4);

}


/*************************************************************

  Scanning
  
 *************************************************************/

void runScan()
{
    uint16_t readings[12];
    ScanningScreen();
    // Get a baseline reading without LEDs active.
    long val = nau.getReading();

    // Reading with no lights on
    Serial.print(val);
    Serial.print(",");

    // Flash the LEDs sequentially and read results from the ADC.
    tft.drawCentreString("Scanning", 120, 120, 4);
    for (int i = 0; i < N_OUTPUTS; i++)
    {
        // Turn on the numerated LED.
        tlc.on(i);
        // Based on comment from Jerry, they tested that 10 ms is fitting for the light to be on, before taking a reading.
        delay(10);
        // Read data from the IR sensor through the ADC.
        long val = nau.getReading();
        // long avgVal = nau.getAverage(5);

        // Serial.print("Read ");
        Serial.print(val);
        Serial.print(",");

        // Calculate the progress in % and combine it with a % sign in a string.
        float progress = float(i) / 8 * 100;
        // String progressOutput = String(progress, 2) + "%";
        String progressOutput = String(val);
        ShowProgress(progressOutput);
        delay(10);
        tlc.off(i);
        delay(10);
    }

    if (!as7341.readAllChannels(readings))
    {
        Serial.println("Error reading all channels!");
        return;
    }

    Serial.print(readings[0]);
    Serial.print(",");
    Serial.print(readings[1]);
    Serial.print(",");
    Serial.print(readings[2]);
    Serial.print(",");
    Serial.print(readings[3]);
    Serial.print(",");
    Serial.print(readings[6]);
    Serial.print(",");
    Serial.print(readings[7]);
    Serial.print(",");
    Serial.print(readings[8]);
    Serial.print(",");
    Serial.print(readings[9]);
    Serial.print(",");
    Serial.print(readings[10]);
    Serial.print(",");
    Serial.print(readings[11]);
    
    // Updates values
    reading855Value  = readings[0] ;  
    reading940Value  = readings[1] ;  
    reading1050Value = readings[2] ; 
    reading1300Value = readings[3] ; 
    reading1460Value = readings[4] ; 
    reading1650Value = readings[5] ; 
    reading1720Value = readings[6] ; 

    SendToFirebase();

    ReadingDone();

  
}








