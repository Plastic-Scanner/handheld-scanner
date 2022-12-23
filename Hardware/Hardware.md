> **Warning**
> Documentation under development.


# Electronics hardware
To make the handheld scanner, the hardware is divided into two modules, connected by a JST SH4 cable following the standard of Sparkfuns Qwiic and Adafruits StemmaQT. The bards are as follows:
- A Scanner module
- A Controller module


# Scanner module
The scanner module has has all NIR LED's, a LED controller to turn them on and off, an ADC to measure the reflectance and a color sensor to estimate the color of the speciment. Via the Qwiic/StemmaQT connector,  its possible to communicate with the color sensor, ADC and LED controller via I2C. The board also has holes for regulair pin connections.
This means that if your goal is to just perform a scan, you just need to connect any controller to the scanner module
![Scanner module](Scanner/Images/RoundScanner-Top.jpg)


# Controller module
The controller module is intended to control the scanner, handle data, give user feedback and provide power for the whole scanner. 
The board is designed to fit a deveopmentboard with the [Adafruit feather layout](https://learn.adafruit.com/adafruit-feather/feather-specification "Link to Sdafruits info on feather design"). 

![Controller board](Controller/Images/ControllerBoard-top.png)

## Why the feather layout?
The adafruit feather is well specified and openly available at [Adafruits website](https://learn.adafruit.com/adafruit-feather/feather-specification "Link to Adafruits info on feather design"), and there is a large variety of boards being designed with the same basic pinout. This gives the opportunity for makers to choose the specific microcontroller that fits their need. All feathers have battery charging and management and run 3.3v. 
Another reason to use a feather layout, is to have a well documented ESP32 board, to minimize problems with missing documentation, and pinouts changing between versions. 

