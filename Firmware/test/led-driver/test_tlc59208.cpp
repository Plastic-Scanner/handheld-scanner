#include "tlc59208.h"
#include <Arduino.h>
#include <Wire.h>
#include <unity.h>

TLC59208 tlc;
static const int N_OUTPUTS = 8;

void blinking_test()
{
    for (int i=0; i<N_OUTPUTS; i++) {
        tlc.on(i);
        delay(500);
        tlc.off(i);
        delay(50);
    }
}

void setup()
{
    delay(2000);        // Startup delay for devices w/o software reset
    Wire.begin();
    tlc.begin();

    UNITY_BEGIN();
    RUN_TEST(blinking_test);
    UNITY_END();
}

void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}