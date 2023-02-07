# A handheld plastic identification tool
A handheld version of the PlasticScanner as a part of a master thesis at DTU

> **Warning**
> This is under development and should not be reproduced before a release is published or if you want do develop it yourself.

## Our mission

We believe that plastic sorting and recycling can be done more effectively. Our handheld Plastic Scanner boosts the process by instantly determining the different plastic types. Our Plastic Scanner can be used in the range from a single household to a large waste management facility.

## The handheld Plastic Scanner

A compact handheld Plastic Scanner that contains a small breakout board with ADC, amplifier, LED controller and a color sensor, that can be effortlessly interfaced using the Qwiic/StemmaQT I2C connectors. 

## Changes from the original Plastic Scanner

The board has the following changes from the original:
- The ADC [ADS1256](https://www.ti.com/product/ADS1256) we interfaced SPI is exchanged for the NAU7802 ADC that we interface via I2C
- [AS7341](https://ams.com/as7341) I2C color sensor added
- 5v changed to 3.3v to comply with Qwiic/stemmaQT guidelines

## Notes
- Holes are M2 holes (iso 7380)
