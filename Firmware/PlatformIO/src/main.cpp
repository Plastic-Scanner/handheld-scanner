// This script is cycles the 8 LEDs and takes a reading using each LED being on. The results are display on the G9 Display.

#include "tlc59208.h" // Small homemade definition library for TLC59208
#include <Wire.h>     // The I2C library
#include <Arduino.h>  // Some Arduino thing
#include <unity.h>    // Unit testing for C
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <math.h>
#include <WiFi.h>
// ML
#include <EloquentTinyML.h>
//#include <eloquent_tinyml/tensorflow.h>
#include "barbie.h"
#define N_INPUTS 12
#define N_RESULTS 1
// in future projects you may need to tweak this value: it's a trial and error process
#define TENSOR_ARENA_SIZE 2 * 1024

#define NUMPIXELS 1

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
NAU7802 nau;
TLC59208 tlc;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);


Eloquent::TinyML::TfLite<N_INPUTS, N_RESULTS, TENSOR_ARENA_SIZE> tf;

static const int N_OUTPUTS = 8; // Number of LEDs to control
static const int RESET = 2;     // //Define reset pin for TLC59208
static const int ADDR = 0x20;   // The TLC59208 I2C address.

void setup()
{
    Serial.begin(115200);
    delay(100); // Startup delay for devices w/o software reset
    Serial.println("boot");
    pinMode(5, INPUT_PULLUP);

    // NEOPIXEL SETUP
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(80);
    pixels.fill(0xFF0000); // Set color of NEO Pixel to RED.
    pixels.show();         // Indicate the device is now running code from setup.

    Wire.begin(); // Define the I2C library for use.
    tlc.begin();  // Define the TLC library for use.

    // NAU7802 SETUP //
    if (nau.begin() == false)
    {
        Serial.println("NAU7802 not detected.");
    }
    Serial.println("NAU7802 detected!");

    // TFT SETUP
    tft.init();
    tft.fillScreen(TFT_DARKGREEN);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("PlasticScanner", 120, 120, 4);
    tft.drawCentreString("Ready", 120, 140, 4);
    tft.setTextDatum(MC_DATUM);

    pixels.fill(0xE6CC00); // Set color of NEO Pixel to YELLOW.
    pixels.show();         // Indicate the device is now done with setup.

    nau.setGain(NAU7802_GAIN_4);
    // float theSampleRate = nau.setSampleRate;
    // Serial.println(theSampleRate);

    // TF INIT
//    tf.begin(barbie);
//    // check if model loaded fine
//    if (!tf.isOk())
//    {
//        Serial.print("ERROR: ");
//        Serial.println(tf.getErrorMessage());
//
//        while (true)
//            delay(1000);
//    }
}


void runScan()
{
    pixels.fill(0x02198B); // Set color of NEO Pixel to BLUE.
    pixels.show();         // Indicate the device is now running a scan.
    // Function to blink the 8 LEDs and collect the associated data from the ADC.
    tft.fillScreen(TFT_DARKGREEN);

    tft.drawCentreString("Scanning", 120, 120, 4);
    Serial.println("Starting scan.");
    // Get a baseline reading without LEDs active.
    long val = nau.getReading();
    Serial.print("Dark reading: ");
    Serial.println(val);

    delay(500);
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
        Serial.println(val);

        // Calculate the progress in % and combine it with a % sign in a string.
        float progress = float(i) / 8 * 100;
        // String progressOutput = String(progress, 2) + "%";
        String progressOutput = String(val);

        // Make space on the screen.
        tft.fillRect(0, 140, 240, 180, TFT_DARKGREEN); // clear a region of the display with the background color
        tft.drawCentreString(progressOutput, 120, 140, 4);
        delay(500);
        tlc.off(i);
        delay(10);
    }

    tft.fillScreen(TFT_DARKGREEN);
    tft.drawCentreString("PP", 120, 100, 4);
    // tft.drawCentreString("...", 120, 140, 4);

    // Waits for button press.
    while (digitalRead(5) == HIGH)
    {
        // Do nothing
    }

    tft.fillScreen(TFT_DARKGREEN);
    tft.drawCentreString("Ready to scan", 120, 110, 4);
}


void loop()
{
    pixels.fill(0x26580F); // Set color of NEO Pixel to GREEN.
    pixels.show();         // Indicate the device is now running loop code.

    // Waits for button press.
    while (digitalRead(5) == HIGH)
    {
        // Do nothing
    }

    // Execute a scan and display the results.
    UNITY_BEGIN();
    RUN_TEST(runScan);
    UNITY_END();

    // Indicate the scan is done.
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
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
