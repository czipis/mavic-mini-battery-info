# DJI Mavic Mini Battery Info


Because Mavic Mini battery has not charge level indicator (like other DJI batteries) with this device you are able to quickly check Mavic Mini battery status, level of charge and other informations.
It reads I2C smart battery chip and report its values. All values are also printed to USB serial line.

![screenshot](https://czipis.eu/images/battery-info-screenshot.jpg)


serial line report example
```
Manufacturer Name: SDI
Device Name: BA01WM160
Chemistry 2044
Manufacturer Data:  
Design Capacity: 2400
Design Voltage: 7200
Manufacture Date (D.M.Y): 20.1.2020
Serial Number: 1U5X02MEXP0ARB
Specification Info: 49
Cycle Count: 10
Voltage: 8.14
Full Charge Capacity: 2302
Remaining Capacity: 2080
Relative Charge(%): 91
Absolute Charge(%): 87
Minutes remaining for full charge: -1
Cell 1 Voltage: 4067
Cell 2 Voltage: 4068
State of Health: 0
Battery Mode (BIN): 0b110000000000001
Battery Status (BIN): 0b11000000
Charging Current: 4750
Charging Voltage: 8400
Temp: 23.75
Current (mA): -131
```

[![Watch the video](https://czipis.eu/images/youtube.png)](https://www.youtube.com/watch?v=iwm-ErN7L9Y)

### required parts
all parts can be purchased on Aliexpress for less than $6

- [Breadboard Jumper wires Cables](https://www.aliexpress.com/item/32725034190.html)
- [1.8" 128x160 65K SPI LCD Display Module ST7735](https://www.aliexpress.com/item/33031122936.html)
- [Arduino Nano ATMEGA328P Mini USB (Nano3 compatible)](https://www.aliexpress.com/item/33013146682.html)

### custom 3D printed case
- [3D printed custom case (Openscad parametrized)](https://www.thingiverse.com/thing:4430174)


### pin connections
![Wiring](./breadboard.png)

 
| Arduino NANO    |  SPI  TFT  |   Mavic Mini battery    |
|-----------------|:----------:|:-----------------------:|
|VIN              |            |            3            |
|GND              |     GND    |            5            |
|5V               |     VDD    |                         | 
|3.3V             |     BLK    |                         |
|A4               |            |            6            |
|A5               |            |            1            |
|D7               |     RST    |                         |
|D8               |     DC     |                         |
|D9               |     CS     |                         |
|ICSP3            |     SCL    |                         | 
|ICSP4            |     SDA    |                         |

## compiling

### required libraries

#### following TFT library needs to be installed from .ZIP archvive

- download [zip](https://github.com/Bodmer/TFT_ST7735/archive/master.zip) and install in Arduino IDE (Sketch -> Include library -> Add .ZIP library)
- [Bodmer TFT_ST7735 v0.17](https://github.com/Bodmer/TFT_ST7735)

#### dependencies which can be installed from Arduino IDE
- (Sketch -> Include library -> Manage libraries...)
- [Adafruit_GFX_Library v1.8.3](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit_BusIO v1.3.0](https://github.com/adafruit/Adafruit_BusIO)

#### modify TFT_ST7735 library to fit in Nano flash memory

modify ${HOME}/Arduino/libraries/TFT_ST7735/User_Setup.h

uncomment following lines at top of the file

``` #define TAB_COLOUR INITR_BLACKTAB ```

comment any other TAB_COLOUR line, it should look like

```
//#define TAB_COLOUR INITB
//#define TAB_COLOUR INITR_GREENTAB
//#define TAB_COLOUR INITR_REDTAB
#define TAB_COLOUR INITR_BLACKTAB
//#define TAB_COLOUR INITR_GREENTAB
```

comment out all fonts except Font 1 and Font 2 (otherwise it will not fit in Nano flash memory)

```
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
//#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
//#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
//#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
//#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
```
