# A handheld plastic identification tool
A handheld version of the PlasticScanner as a part of a masterthesis at DTU

> **Warning**
<<<<<<< HEAD
> This is under development, and should not be reproduced before a release is published, or if you want do develop it yourself
=======
> This is under development, and should not be reproduced before a release is published

## Idea

To make a small breakoutboard with ADC, Amplifier, LED contorl and a color sensor, that is easy to interface via a Qwiic/StemmaQT I2C connector. 

## Changes from the original plastic scanner
The board has the following changes from the original:
- The ADC ADS1256 we interfaced SPI is changed to the NAU7802 ADC that we interface via I2C
- AS7341 I2C color sensor added
- 5v changed to 3.3v to comply with Qwiic/stemmaQT guidelines
>>>>>>> 8ba2b8f (List of changes)
