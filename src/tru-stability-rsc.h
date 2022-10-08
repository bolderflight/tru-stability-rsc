/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2022 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#ifndef TRU_STABILITY_RSC_SRC_TRU_STABILITY_RSC_H_  // NOLINT
#define TRU_STABILITY_RSC_SRC_TRU_STABILITY_RSC_H_

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <cstddef>
#include <cstdint>
#include "core/core.h"
#endif

namespace bfs {

class TruStabilityRsc {
 public:
  enum Mode : uint8_t {
    NORMAL_MODE_20HZ =    0b00000100,
    NORMAL_MODE_45HZ =    0b00100100,
    NORMAL_MODE_90HZ =    0b01000100,
    NORMAL_MODE_175HZ =   0b01100100,
    NORMAL_MODE_330HZ =   0b10000100,
    NORMAL_MODE_600HZ =   0b10100100,
    NORMAL_MODE_1000HZ =  0b11000100,
    FAST_MODE_40HZ =      0b00010100,
    FAST_MODE_90HZ =      0b00110100,
    FAST_MODE_180HZ =     0b01010100,
    FAST_MODE_350HZ =     0b01110100,
    FAST_MODE_660HZ =     0b10010100,
    FAST_MODE_1200HZ =    0b10110100,
    FAST_MODE_2000HZ =    0b11010100
  };
  TruStabilityRsc() {}
  TruStabilityRsc(SPIClass *spi, const uint8_t cs_ee, const uint8_t cs_adc) :
                  spi_(spi), cs_ee_(cs_ee), cs_adc_(cs_adc) {}
  void Config(SPIClass *spi, const uint8_t cs_ee, const uint8_t cs_adc);
  void Begin();
  void Read();
  void ConfigMode(const Mode rate);
  inline void temp_srd(const uint8_t srd) {srd_ = srd;}
  inline uint8_t temp_srd() const {return srd_;}
  inline Mode mode() const {return static_cast<Mode>(mode_);}
  inline const char * prod_name() const {return prod_name_;}
  inline const char * serial_num() const {return serial_num_;}
  inline float pres_pa() const {return pres_pa_;}
  inline float die_temp_c() const {return temp_c_;}

 private:
  SPIClass *spi_;
  uint8_t cs_ee_, cs_adc_;
  static constexpr int32_t SPI_CLOCK_ = 5000000;
  static constexpr uint8_t EAD_EEPROM_ = 0x03;
  static constexpr uint8_t WREG_ = 0x40;
  static constexpr uint16_t EEPROM_MASK_ = 0x100;
  static constexpr uint8_t ADC_REG_MASK_ = 0x0C;
  static constexpr uint8_t ADC_COUNT_MASK_ = 0x03;
  uint8_t reg_lsb_, reg_msb_;
  /* Registers */
  static constexpr uint16_t PROD_NAME_REG_ = 0;
  static constexpr uint8_t PROD_NAME_LEN_ = 16;
  static constexpr uint16_t SERIAL_NUM_REG_ = 16;
  static constexpr uint8_t SERIAL_NUM_LEN_ = 11;
  static constexpr uint16_t PRES_RANGE_REG_ = 27;
  static constexpr uint8_t PRES_RANGE_LEN_ = 4;
  static constexpr uint16_t PRES_MIN_REG_ = 31;
  static constexpr uint8_t PRES_MIN_LEN_ = 4;
  static constexpr uint16_t PRES_UNIT_REG_ = 35;
  static constexpr uint8_t PRES_UNIT_LEN_ = 5;
  static constexpr uint16_t PRES_UNIT_REF_REG_ = 40;
  static constexpr uint8_t PRES_UNIT_REF_LEN_ = 1;
  static constexpr uint16_t ADC_CONFIG_0_REG_ = 61;
  static constexpr uint16_t ADC_CONFIG_1_REG_ = 63;
  static constexpr uint16_t ADC_CONFIG_2_REG_ = 65;
  static constexpr uint16_t ADC_CONFIG_3_REG_ = 67;
  static constexpr uint8_t ADC_CONFIG_LEN_ = 1;
  static constexpr uint16_t OFFSET_COEFF_0_REG_ = 130;
  static constexpr uint16_t OFFSET_COEFF_1_REG_ = 134;
  static constexpr uint16_t OFFSET_COEFF_2_REG_ = 138;
  static constexpr uint16_t OFFSET_COEFF_3_REG_ = 142;
  static constexpr uint8_t OFFSET_COEFF_LEN_ = 4;
  static constexpr uint16_t SPAN_COEFF_0_REG_ = 210;
  static constexpr uint16_t SPAN_COEFF_1_REG_ = 214;
  static constexpr uint16_t SPAN_COEFF_2_REG_ = 218;
  static constexpr uint16_t SPAN_COEFF_3_REG_ = 222;
  static constexpr uint8_t SPAN_COEFF_LEN_ = 4;
  static constexpr uint16_t SHAPE_COEFF_0_REG_ = 290;
  static constexpr uint16_t SHAPE_COEFF_1_REG_ = 294;
  static constexpr uint16_t SHAPE_COEFF_2_REG_ = 298;
  static constexpr uint16_t SHAPE_COEFF_3_REG_ = 302;
  static constexpr uint8_t SHAPE_COEFF_LEN_ = 4;
  static constexpr uint8_t ADC_RESET_ = 0x06;
  static constexpr uint8_t ADC_CONF_REG_ = 0x00;
  static constexpr uint8_t ADC_USR_CONF_REG_ = 0x01;
  static constexpr uint8_t DATA_CONV_START_ = 0x10;
  static constexpr uint8_t TEMP_READING_ = 0x02;
  static constexpr uint8_t PRES_READING_ = 0x00;
  /* Config */
  uint8_t mode_ = static_cast<uint8_t>(NORMAL_MODE_20HZ);
  uint8_t srd_ = 0, cnt_ = 0;
  uint8_t buf_[16];
  char prod_name_[PROD_NAME_LEN_ + 1] = {'\0'};
  char serial_num_[SERIAL_NUM_LEN_ + 1] = {'\0'};
  char pres_unit_[PRES_UNIT_LEN_ + 1] = {'\0'};
  char pres_unit_ref_[PRES_UNIT_REF_LEN_ + 1] = {'\0'};
  uint8_t adc_config_[4];
  float pres_range_;
  float pres_min_;
  float offset_matrix_[4];
  float span_matrix_[4];
  float shape_matrix_[4];
  enum Meas {
    TEMP,
    PRES
  } meas_;
  /* Data */
  uint16_t traw_, tcomp_;
  int32_t praw_;
  float trawf_, prawf_;
  float temp_c_;
  float pint1_, pint2_, pcompfs_;
  float pres_, pres_pa_;
  float conv_to_pa_;
  /* Utility functions */
  float ConvertTemp(const uint8_t count, uint8_t * const data);
  float ConvertPres(const uint8_t count, uint8_t * const data);
  void TempDataConv();
  void PresDataConv();
  void ReadData(const uint8_t count, uint8_t * const data);
  void ResetAdc();
  void WriteAdcRegisters(const uint16_t addr, const uint8_t count,
                         uint8_t * const data);
  void ReadEepromRegisters(const uint16_t addr, const uint8_t count,
                           uint8_t * const data);
};

}  // namespace bfs

#endif  // TRU_STABILITY_RSC_SRC_TRU_STABILITY_RSC_H_ NOLINT
