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

#include "tru-stability-rsc.h"  // NOLINT

namespace bfs {

void TruStabilityRsc::Config(SPIClass *spi, const uint8_t cs_ee,
                             const uint8_t cs_adc) {
  spi_ = spi;
  cs_ee_ = cs_ee;
  cs_adc_ = cs_adc;
}

void TruStabilityRsc::Begin() {
  /* Configure the CS pins */
  pinMode(cs_ee_, OUTPUT);
  pinMode(cs_adc_, OUTPUT);
  digitalWrite(cs_ee_, HIGH);
  digitalWrite(cs_adc_, HIGH);
  /* Get the product name */
  ReadEepromRegisters(PROD_NAME_REG_, PROD_NAME_LEN_, buf_);
  memcpy(prod_name_, buf_, PROD_NAME_LEN_);
  /* Get the serial number */
  ReadEepromRegisters(SERIAL_NUM_REG_, SERIAL_NUM_LEN_, buf_);
  memcpy(serial_num_, buf_, SERIAL_NUM_LEN_);
  /* Get the pressure range */
  ReadEepromRegisters(PRES_RANGE_REG_, PRES_RANGE_LEN_, buf_);
  memcpy(&pres_range_, buf_, PRES_RANGE_LEN_);
  /* Get the pressure minimum */
  ReadEepromRegisters(PRES_MIN_REG_, PRES_MIN_LEN_, buf_);
  memcpy(&pres_min_, buf_, PRES_MIN_LEN_);
  /* Get the pressure unit */
  ReadEepromRegisters(PRES_UNIT_REG_, PRES_UNIT_LEN_, buf_);
  memcpy(pres_unit_, buf_, PRES_UNIT_LEN_);
  for (int8_t i = 0; pres_unit_[i]; i++) {
    pres_unit_[i] = tolower(pres_unit_[i]);
  }
  if (strcmp(pres_unit_, "inh2o") == 0) {
    conv_to_pa_ = 248.84f;
  } else if (strcmp(pres_unit_, "psi") == 0) {
    conv_to_pa_ = 0.45359237f * 9.80665f / 0.0254f / 0.0254f;
  } else if (strcmp(pres_unit_, "mbar") == 0) {
    conv_to_pa_ = 100.0f;
  } else if (strcmp(pres_unit_, "bar") == 0) {
    conv_to_pa_ = 100000.0f;
  } else if (strcmp(pres_unit_, "pa") == 0) {
    conv_to_pa_ = 1.0f;
  } else if (strcmp(pres_unit_, "kpa") == 0) {
    conv_to_pa_ = 1000.0f;
  } else {
    return false;
  }
  /* Get the pressure unit reference */
  ReadEepromRegisters(PRES_UNIT_REF_REG_, PRES_UNIT_REF_LEN_, buf_);
  memcpy(pres_unit_ref_, buf_, PRES_UNIT_REF_LEN_);
  /* Get the ADC config settings */
  ReadEepromRegisters(ADC_CONFIG_0_REG_, ADC_CONFIG_LEN_, buf_);
  memcpy(&adc_config_[0], buf_, ADC_CONFIG_LEN_);
  ReadEepromRegisters(ADC_CONFIG_1_REG_, ADC_CONFIG_LEN_, buf_);
  memcpy(&adc_config_[1], buf_, ADC_CONFIG_LEN_);
  ReadEepromRegisters(ADC_CONFIG_2_REG_, ADC_CONFIG_LEN_, buf_);
  memcpy(&adc_config_[2], buf_, ADC_CONFIG_LEN_);
  ReadEepromRegisters(ADC_CONFIG_3_REG_, ADC_CONFIG_LEN_, buf_);
  memcpy(&adc_config_[3], buf_, ADC_CONFIG_LEN_);
  /* Get the polynomial coefficients */
  ReadEepromRegisters(OFFSET_COEFF_0_REG_, OFFSET_COEFF_LEN_, buf_);
  memcpy(&offset_matrix_[0], buf_, OFFSET_COEFF_LEN_);
  ReadEepromRegisters(OFFSET_COEFF_1_REG_, OFFSET_COEFF_LEN_, buf_);
  memcpy(&offset_matrix_[1], buf_, OFFSET_COEFF_LEN_);
  ReadEepromRegisters(OFFSET_COEFF_2_REG_, OFFSET_COEFF_LEN_, buf_);
  memcpy(&offset_matrix_[2], buf_, OFFSET_COEFF_LEN_);
  ReadEepromRegisters(OFFSET_COEFF_3_REG_, OFFSET_COEFF_LEN_, buf_);
  memcpy(&offset_matrix_[3], buf_, OFFSET_COEFF_LEN_);
  /* Get the span coefficients */
  ReadEepromRegisters(SPAN_COEFF_0_REG_, SPAN_COEFF_LEN_, buf_);
  memcpy(&span_matrix_[0], buf_, SPAN_COEFF_LEN_);
  ReadEepromRegisters(SPAN_COEFF_1_REG_, SPAN_COEFF_LEN_, buf_);
  memcpy(&span_matrix_[1], buf_, SPAN_COEFF_LEN_);
  ReadEepromRegisters(SPAN_COEFF_2_REG_, SPAN_COEFF_LEN_, buf_);
  memcpy(&span_matrix_[2], buf_, SPAN_COEFF_LEN_);
  ReadEepromRegisters(SPAN_COEFF_3_REG_, SPAN_COEFF_LEN_, buf_);
  memcpy(&span_matrix_[3], buf_, SPAN_COEFF_LEN_);
  /* Get the shape coefficients */
  ReadEepromRegisters(SHAPE_COEFF_0_REG_, SHAPE_COEFF_LEN_, buf_);
  memcpy(&shape_matrix_[0], buf_, SHAPE_COEFF_LEN_);
  ReadEepromRegisters(SHAPE_COEFF_1_REG_, SHAPE_COEFF_LEN_, buf_);
  memcpy(&shape_matrix_[1], buf_, SHAPE_COEFF_LEN_);
  ReadEepromRegisters(SHAPE_COEFF_2_REG_, SHAPE_COEFF_LEN_, buf_);
  memcpy(&shape_matrix_[2], buf_, SHAPE_COEFF_LEN_);
  ReadEepromRegisters(SHAPE_COEFF_3_REG_, SHAPE_COEFF_LEN_, buf_);
  memcpy(&shape_matrix_[3], buf_, SHAPE_COEFF_LEN_);
  /* Send ADC corrections */
  WriteAdcRegisters(ADC_CONF_REG_, 4, adc_config_);
  /* Grab a temperature sample */
  TempDataConv();
  delay(65);
  ReadData(3, buf_);
  temp_c_ = ConvertTemp(3, buf_);
  PresDataConv();
  delay(65);
  ReadData(3, buf_);
  pres_ = ConvertPres(3, buf_);
  pres_pa_ = pres_ * conv_to_pa_;
}

void TruStabilityRsc::ConfigMode(const Mode rate) {
  mode_ = static_cast<uint8_t>(rate);
  TempDataConv();
  delay(65);
  ReadData(3, buf_);
  temp_c_ = ConvertTemp(3, buf_);
  PresDataConv();
  delay(65);
  ReadData(3, buf_);
  pres_ = ConvertPres(3, buf_);
  pres_pa_ = pres_ * conv_to_pa_;
}

float TruStabilityRsc::ConvertTemp(const uint8_t count, uint8_t * const data) {
  traw_ = (static_cast<uint16_t>(data[0]) << 8 | data[1]) >> 2;
  if (traw_ & 0x2000) {
    tcomp_ = traw_ << 2;
    tcomp_ = ~tcomp_;
    tcomp_ = tcomp_ >> 2;
    tcomp_++;
    return static_cast<float>(tcomp_) * -0.03125f;
  } else {
    return static_cast<float>(traw_) * 0.03125f;
  }
}

float TruStabilityRsc::ConvertPres(const uint8_t count, uint8_t * const data) {
  praw_ = static_cast<int32_t>(data[0]) << 24 |
          static_cast<int32_t>(data[1]) << 16  |
          static_cast<int32_t>(data[2]) << 8;
  praw_ /= 256;
  prawf_ = static_cast<float>(praw_);
  trawf_ = static_cast<float>(traw_);
  pint1_ = prawf_ - (offset_matrix_[3] * trawf_ * trawf_ * trawf_ +
                     offset_matrix_[2] * trawf_ * trawf_ +
                     offset_matrix_[1] * trawf_ +
                     offset_matrix_[0]);
  pint2_ = pint1_ / (span_matrix_[3] * trawf_ * trawf_ * trawf_ +
                     span_matrix_[2] * trawf_ * trawf_ +
                     span_matrix_[1] * trawf_ +
                     span_matrix_[0]);
  pcompfs_ = shape_matrix_[3] * pint2_ * pint2_ * pint2_ +
             shape_matrix_[2] * pint2_ * pint2_ +
             shape_matrix_[1] * pint2_ +
             shape_matrix_[0];
  return pcompfs_ * pres_range_ + pres_min_;
}

void TruStabilityRsc::Read() {
  ReadData(3, buf_);
  if (meas_ == TEMP) {
    temp_c_ = ConvertTemp(3, buf_);
    PresDataConv();
  } else {
    pres_ = ConvertPres(3, buf_);
    pres_pa_ = pres_ * conv_to_pa_;
    cnt_++;
  }
  if (cnt_ > srd_) {
    cnt_ = 0;
    TempDataConv();
  }
}

void TruStabilityRsc::ReadData(const uint8_t count, uint8_t * const data) {
  spi_->beginTransaction(SPISettings(SPI_CLOCK_, MSBFIRST, SPI_MODE1));
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, LOW);
  #else
  digitalWrite(cs_adc_, LOW);
  #endif
  spi_->transfer(DATA_CONV_START_);
  spi_->transfer(data, count);
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, HIGH);
  #else
  digitalWrite(cs_adc_, HIGH);
  #endif
  spi_->endTransaction();
}

void TruStabilityRsc::TempDataConv() {
  reg_msb_ = mode_ | TEMP_READING_;
  WriteAdcRegisters(ADC_USR_CONF_REG_, 1, &reg_msb_);
  meas_ = TEMP;
}

void TruStabilityRsc::PresDataConv() {
  reg_msb_ = mode_ | PRES_READING_;
  WriteAdcRegisters(ADC_USR_CONF_REG_, 1, &reg_msb_);
  meas_ = PRES;
}

void TruStabilityRsc::ResetAdc() {
  spi_->beginTransaction(SPISettings(SPI_CLOCK_, MSBFIRST, SPI_MODE1));
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, LOW);
  #else
  digitalWrite(cs_adc_, LOW);
  #endif
  spi_->transfer(ADC_RESET_);
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, HIGH);
  #else
  digitalWrite(cs_adc_, HIGH);
  #endif
  spi_->endTransaction();
}

void TruStabilityRsc::WriteAdcRegisters(const uint16_t addr,
                                        const uint8_t count,
                                        uint8_t * const data) {
  reg_lsb_ = WREG_ | ((addr << 2) & ADC_REG_MASK_) |
             ((count - 1) & ADC_COUNT_MASK_);
  spi_->beginTransaction(SPISettings(SPI_CLOCK_, MSBFIRST, SPI_MODE1));
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, LOW);
  #else
  digitalWrite(cs_adc_, LOW);
  #endif
  spi_->transfer(reg_lsb_);
  spi_->transfer(data, count);
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_adc_, HIGH);
  #else
  digitalWrite(cs_adc_, HIGH);
  #endif
  spi_->endTransaction();
}

void TruStabilityRsc::ReadEepromRegisters(const uint16_t addr,
                                          const uint8_t count,
                                          uint8_t * const data) {
  reg_msb_ = EAD_EEPROM_ | ((addr & EEPROM_MASK_) >> 5);
  reg_lsb_ = addr & 0xFF;
  spi_->beginTransaction(SPISettings(SPI_CLOCK_, MSBFIRST, SPI_MODE0));
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_ee_, LOW);
  #else
  digitalWrite(cs_ee_, LOW);
  #endif
  spi_->transfer(reg_msb_);
  spi_->transfer(reg_lsb_);
  spi_->transfer(data, count);
  #if defined(TEENSYDUINO)
  digitalWriteFast(cs_ee_, HIGH);
  #else
  digitalWrite(cs_ee_, HIGH);
  #endif
  spi_->endTransaction();
}

}  // namespace bfs
