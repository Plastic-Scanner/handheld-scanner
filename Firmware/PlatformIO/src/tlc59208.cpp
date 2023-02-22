#include <tlc59208.h>
#include <assert.h>
#include <Arduino.h>
#include <Wire.h>

static const int RESET = 2;
static const int ADDR  = 0x20;
static const int N_OUTPUTS = 8;

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
    assert((percents >= 0) && (percents <=100));

    uint8_t reg = 0x02 + output;
    uint8_t pwm = (percents * 255)/100;
    
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
    Wire.write(0x80);   // Control: auto-increment + register address
    Wire.write(0x81);   // 00h: MODE1       Auto-increment, disable sleep mode, allcall address enabled
    Wire.write(0x00);   // 01h: MODE2
    Wire.write(0x00);   // 02h: PWM0
    Wire.write(0x00);   // 03h: PWM1
    Wire.write(0x00);   // 04h: PWM2
    Wire.write(0x00);   // 05h: PWM3
    Wire.write(0x00);   // 06h: PWM4
    Wire.write(0x00);   // 07h: PWM5
    Wire.write(0x00);   // 08h: PWM6
    Wire.write(0x00);   // 09h: PWM7
    Wire.write(0x00);   // 0Ah: GRPPWM
    Wire.write(0x00);   // 0Bh: GRPFREQ
    Wire.write(0xAA);   // 0Ch: LEDOUT0         LEDs controlled via PWM registers
    Wire.write(0xAA);   // 0Dh: LEDOUT1         LEDs controlled via PWM registers
    Wire.write(0x00);   // 0Eh: SUBADR1
    Wire.write(0x00);   // 0Fh: SUBADR2
    Wire.write(0x00);   // 10h: SUBADR3
    Wire.write(0x00);   // 11h: ALLCALLADR
    bool ret = Wire.endTransmission();
    
    assert(ret == 0);   // Fail if data not ACKed
}


void TLC59208::on(int output)
{
    setBrightness(output, 100);
}

void TLC59208::off(int output)
{
    setBrightness(output, 0);
}
