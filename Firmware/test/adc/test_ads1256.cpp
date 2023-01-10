#include "ads1256.h"
#include <Arduino.h>
#include <SPI.h>
#include <unity.h>

ADS1256 adc;

void test_read_id()
{
    for (int i=0; i<10; i++) {
        TEST_ASSERT_EQUAL_INT(3, adc.read_id());
    }
}

void setup()
{
    delay(2000);        // Startup delay for devices w/o software reset
    SPI.begin();
    adc.begin();

    UNITY_BEGIN();
    RUN_TEST(test_read_id);
    UNITY_END();
}

void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}