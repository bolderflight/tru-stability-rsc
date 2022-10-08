[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

![Bolder Flight Systems Logo](img/logo-words_75.png) &nbsp; &nbsp; ![Arduino Logo](img/arduino_logo_75.png)

# TruStabilityRsc
This library communicates with [Honeywell TruStability RSC](https://sps.honeywell.com/us/en/products/advanced-sensing-technologies/healthcare-sensing/board-mount-pressure-sensors/trustability-rsc-series) pressure transducers. These sensors come in a wide variety of pressure ranges, measure pressure with 24 bit analog to digital converters, and communicate over SPI. This library is compatible with Arduino and CMake build systems.
   * [License](LICENSE.md)
   * [Changelog](CHANGELOG.md)
   * [Contributing guide](CONTRIBUTING.md)

# Installation

## Arduino
Use the Arduino Library Manager to install this library or clone to your Arduino/libraries folder. This library is added as:

```C++
#include "tru-stability-rsc.h"
```

Example Arduino executables are located in: *examples/arduino/*. Teensy 3.x, 4.x, and LC devices are used for testing under Arduino and this library should be compatible with other Arduino devices.

## CMake
CMake is used to build this library, which is exported as a library target called *tru-stability-rsc*. This library is added as:

```C++
#include "tru-stability-rsc.h"
```

The library can be also be compiled stand-alone using the CMake idiom of creating a *build* directory and then, from within that directory issuing:

```
cmake .. -DMCU=MK66FX1M0
make
```

This will build the library and example executables called *spi_example*. The example executable source file is located at *examples/cmake*. Notice that the *cmake* command includes a define specifying the microcontroller the code is being compiled for. This is required to correctly configure the code, CPU frequency, and compile/linker options. The available MCUs are:
   * MK20DX128
   * MK20DX256
   * MK64FX512
   * MK66FX1M0
   * MKL26Z64
   * IMXRT1062_T40
   * IMXRT1062_T41
   * IMXRT1062_MMOD

These are known to work with the same packages used in Teensy products. Also switching packages is known to work well, as long as it's only a package change.

The example targets create executables for communicating with the sensor using SPI communication. Each target also has a *_hex*, for creating the hex file to upload to the microcontroller, and an *_upload* for using the [Teensy CLI Uploader](https://www.pjrc.com/teensy/loader_cli.html) to flash the Teensy. Please note that instructions for setting up your build environment can be found in our [build-tools repo](https://github.com/bolderflight/build-tools).

# Namespace
This library is within the namespace *bfs*.

# TruStabilityRsc

## Methods

**TruStabilityRsc()** Default constructor, requires calling the *Config* method to setup the SPI bus and chip select pin.

**TruStabilityRsc(SPIClass &ast;spi, const uint8_t cs_ee, const uint8_t cs_adc)** Creates a *TruStabilityRsc* object. A pointer to the SPI bus object is passed along with the chip select pin of the sensor EEPROM and the chip select pin of the sensor ADC.

```C++
/*
* TruStability RSC sensor on SPI with EEPROM CS on pin 20 and ADC CS on pin 19
*/
bfs::TruStabilityRsc pres(&SPI, 20, 19);
```

**void Config(SPIClass &ast;spi, const uint8_t cs_ee, const uint8_t cs_adc)** This is required when using the default constructor and sets up the SPI bus and chip select pins.

**void Begin()** Initializes communication with the sensor. The communication bus is not initialized within this library and must be initialized seperately; this enhances compatibility with other sensors that may on the same bus.

```C++
/* Initialize sensor */
SPI.begin();
pres.Begin();
```

**void ConfigMode(const Mode rate)** Configures the sampling mode; whether the sensor is in normal or fast mode and the sampling rate.

| Enum | Description |
| --- | --- |
| NORMAL_MODE_20HZ | Normal mode, 20Hz |
| NORMAL_MODE_45HZ | Normal mode, 45Hz |
| NORMAL_MODE_90HZ | Normal mode, 90Hz |
| NORMAL_MODE_175HZ | Normal mode, 175Hz |
| NORMAL_MODE_330HZ | Normal mode, 330Hz |
| NORMAL_MODE_600HZ | Normal mode, 600Hz |
| NORMAL_MODE_1000HZ | Normal mode, 1000Hz |
| FAST_MODE_40HZ | Fast mode, 40Hz |
| FAST_MODE_90HZ | Fast mode, 90Hz |
| FAST_MODE_180HZ | Fast mode, 180Hz |
| FAST_MODE_350HZ | Fast mode, 350Hz |
| FAST_MODE_660HZ | Fast mode, 660Hz |
| FAST_MODE_1200HZ | Fast mode, 1200Hz |
| FAST_MODE_2000HZ | Fast mode, 2000Hz |

**Mode mode()** Returns the configured mode.

**void temp_srd(const uint8_t srd)** Configures the temperature sample rate divider. Die temperature is used to compensate the pressure measurement; however, die temperature typically changes at a much slower rate than pressure. This sample rate divider enables configuring how often a temperature measurement is taken. If the srd is set to 0, calling the *Read* method will alternate between taking pressure and die temperature measurements. Setting the srd to 1 means two pressure measurements will be taken before a temperature measurement.

```C++
pres.temp_srd(10);
```

**uint8_t temp_srd()** Returns the configured srd.

**const char * prod_name()** Returns the pressure transducer name.

**const char * serial_num()** Returns the pressure transducer serial number.

**void Read()** Reads the sensor and stores the latest pressure or temperature measurement in the object. Note that there is no feedback about whether the sensor data is new. The data ready interrupt pin can be used to indicate this or timing based on the configured sampling mode.

```C++
pres.Read();
```

**float pres_pa()** Returns the latest pressure measurement from the object, Pa.

```C++
pres.pres_pa();
```

**float die_temp_c()** Returns the latest die temperature measurement from the object, C.

```C++
pres.die_temp_c();
```
