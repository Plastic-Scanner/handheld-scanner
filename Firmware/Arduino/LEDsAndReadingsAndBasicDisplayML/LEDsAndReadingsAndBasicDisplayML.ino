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
// #include <WiFi.h>
#include <Adafruit_AS7341.h>
// ML
#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "barbie.h"
#define N_INPUTS 17
#define N_RESULTS 2

// Try out huge size  -> model is 20k bytes
#define TENSOR_ARENA_SIZE 25 * 1024


#define NUMPIXELS 1

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
NAU7802 nau;
TLC59208 tlc;
Adafruit_AS7341 as7341;
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_RESULTS, TENSOR_ARENA_SIZE> tf;

static const int N_OUTPUTS = 8; // Number of LEDs to control
static const int RESET = 2;     // //Define reset pin for TLC59208
static const int ADDR = 0x20;   // The TLC59208 I2C address.

void setup()
{
    Serial.begin(115200);
    delay(800); // Startup delay for devices w/o software reset. Also required for serial.println to actually get to output.
    Serial.println("boot");
    pinMode(5, INPUT_PULLUP);

    // NEOPIXEL SETUP
    pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    pixels.setBrightness(80);
    pixels.fill(0xFF0000); // Set color of NEO Pixel to RED.
    pixels.show();         // Indicate the device is now running code from setup.

    Wire.begin(); // Define the I2C library for use.
    tlc.begin();  // Define the TLC library for use.

    // TFT SETUP
    tft.init();
    tft.fillScreen(TFT_DARKGREEN);
    tft.setTextColor(TFT_WHITE);
    tft.drawCentreString("PlasticScanner", 120, 120, 4);

    // NAU7802 SETUP //
    if (nau.begin() == false)
    {
        Serial.println("NAU7802 not detected.");
    }
    Serial.println("NAU7802 detected!");

    nau.setGain(NAU7802_GAIN_1);
    // float theSampleRate = nau.setSampleRate;
    // Serial.println(theSampleRate);

    // AS7341 setup
    if (!as7341.begin())
    {
        Serial.println("Could not find AS7341");
        while (1)
        {
            delay(10);
        }
    }
    as7341.setATIME(100);
    as7341.setASTEP(999);
    // Max output value appears to be 65535
    as7341.setGain(AS7341_GAIN_16X);

    // TF INIT
    tf.begin(barbie);
    // check if model loaded fine
    if (!tf.isOk())
    {
        Serial.print("ERROR: ");
        Serial.println(tf.getErrorMessage());
    }
    else
    {
        Serial.println("Such wow");
    }


    // Show the scanner is booted and ready. 
    tft.drawCentreString("Ready", 120, 140, 4);
    tft.setTextDatum(MC_DATUM);

    pixels.fill(0xE6CC00); // Set colour of NEO Pixel to YELLOW. The colour is only really shown if the loop() code fails to run.  
    pixels.show();         // Indicate the device is now done with setup.

}

void runScan()
{
    pixels.fill(0x02198B); // Set color of NEO Pixel to BLUE.
    pixels.show();         // Indicate the device is now running a scan.
    // Function to blink the 8 LEDs and collect the associated data from the ADC.
    tft.fillScreen(TFT_DARKGREEN);

    long irReadings[9];
    double allReadings[19];

    tft.drawCentreString("Scanning", 120, 120, 4);
    Serial.println("Starting scan.");
    // Get a baseline reading without LEDs active.
    long val = nau.getReading();
    Serial.println(val);
    irReadings[0] = val;

    delay(10);
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
        irReadings[i + 1] = val;

        Serial.println(val);

        // Calculate the progress in % and combine it with a % sign in a string.
        // float progress = float(i) / 8 * 100;
        // String progressOutput = String(progress, 2) + "%";
        String progressOutput = String(val);

        // Make space on the screen.
        tft.fillRect(0, 140, 240, 180, TFT_DARKGREEN); // clear a region of the display with the background color
        tft.drawCentreString(progressOutput, 120, 140, 4);
        delay(10);
        tlc.off(i);
        delay(10);
    }

    //   Serial.println("Output of array");
    //   for (int i = 0; i < 9; i++)
    //   {
    //       Serial.println(irReadings[i]);
    //   }

    // Color sensor readings
    uint16_t colorReadings[12];

    as7341.setLEDCurrent(30); // 30mA
    as7341.enableLED(true);
    delay(10);
    as7341.readAllChannels(colorReadings);
    as7341.enableLED(false);

    Serial.println("JUST COLOR");
    // Output of colorReadings array
    for (int i = 0; i < 12; i++)
    {
        Serial.print(colorReadings[i]);
        Serial.print(",");
    }

    Serial.println("All readings in an array");

    // Adding IR values
    int offset = 0;
    for (int i = 0; i < 9; i++)
    {
        allReadings[i] = irReadings[i];
    }

    // Adding color values
    for (int i = 9; i < 19; i++)
    {
        if (i == 12) // skip the two duplicate values from colorReadings.
        {
            offset = 2;
        }
        allReadings[i] = colorReadings[i - 9 + offset];
    }
    // Display the array
    for (int i = 0; i < 19; i++)
    {
        Serial.print(allReadings[i]);
        Serial.print(", ");
    }

    // BEERS LAW
    Serial.println("");
    Serial.println("LAW");  

    // OG first two rows. 
    // (allReadings[2] / allReadings[0]),
    // (allReadings[2] / allReadings[1]),

    double law[17] = {
        (allReadings[2] / allReadings[0]),
        (allReadings[3] / allReadings[2]),
        (allReadings[4] / allReadings[3]),
        (allReadings[5] / allReadings[4]),
        (allReadings[6] / allReadings[5]),
        (allReadings[7] / allReadings[6]),
        (allReadings[8] / allReadings[7]),
        (allReadings[9] / allReadings[8]),
        (allReadings[10] / allReadings[9]),
        (allReadings[11] / allReadings[10]),
        (allReadings[12] / allReadings[11]),
        (allReadings[13] / allReadings[12]),
        (allReadings[14] / allReadings[13]),
        (allReadings[15] / allReadings[14]),
        (allReadings[16] / allReadings[15]),
        (allReadings[17] / allReadings[16]),
        (allReadings[18] / allReadings[17]),
    };

    Serial.println("");
    for (int i = 0; i < 17; i++)
    {
        Serial.print(law[i],14);
        Serial.print(", ");
    }

    // Find min and max values
    double maxVal = law[0];
    double minVal = law[0];
    Serial.print("Size of myArray is: ");
    Serial.println(sizeof(law));

    for (int i = 0; i < (sizeof(law) / sizeof(law[0])); i++)
    {
        maxVal = max(law[i], maxVal);
        minVal = min(law[i], minVal);
    }

    Serial.println("min:");
    Serial.println(minVal);
    Serial.println("max:");
    Serial.println(maxVal);

    Serial.println("");

    // Now Normalizing the values to input to the ML model.
    float input[17] = {
        (law[0] - minVal) / (maxVal - minVal),
        (law[1] - minVal) / (maxVal - minVal),
        (law[2] - minVal) / (maxVal - minVal),
        (law[3] - minVal) / (maxVal - minVal),
        (law[4] - minVal) / (maxVal - minVal),
        (law[5] - minVal) / (maxVal - minVal),
        (law[6] - minVal) / (maxVal - minVal),
        (law[7] - minVal) / (maxVal - minVal),
        (law[8] - minVal) / (maxVal - minVal),
        (law[9] - minVal) / (maxVal - minVal),
        (law[10] - minVal) / (maxVal - minVal),
        (law[11] - minVal) / (maxVal - minVal),
        (law[12] - minVal) / (maxVal - minVal),
        (law[13] - minVal) / (maxVal - minVal),
        (law[14] - minVal) / (maxVal - minVal),
        (law[15] - minVal) / (maxVal - minVal),
        (law[16] - minVal) / (maxVal - minVal),
    };

    // Print normalized values
    Serial.println("Normalized values");
    for (int i = 0; i < 17; i++)
    {
        Serial.print(input[i],14);
        Serial.print(", ");
    }

    // PREDICTION
    Serial.println("Predicting");
    // float input[12] = {0.0597,0.236,0.7407,0.2757,0.9117,0.297,0.125,0.7302,0.5958,0.2311,0.2475,0.4639}; // OG test vals
//    float input[12] = {0.0597, 0.246, 0.7407, 0.2757, 0.9117, 0.297, 0.125, 0.7302, 0.5958, 0.2311, 0.2475, 0.4639}; //  With small change
    float predicted = tf.predict(input);
    Serial.print("\t predicted: ");
    Serial.println(predicted);
    if (predicted > 0.75) {
        Serial.println("I think it is PS!");
    } else if (predicted < 0.25) {
        Serial.println("I think it is PP!");
    }   else {
        Serial.println("What the hell is this?!");
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

// THE GREAT LOOP

void loop()
{
    // void loop() needs to be at the end of the file, otherwise runScan() is not recognized. This is somehow a result of loading the barbie.h file.
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
}
