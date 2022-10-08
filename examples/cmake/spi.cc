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

#include "tru-stability-rsc.h"


/*
* TruStability RSC sensor on SPI with EEPROM CS on pin 20 and ADC CS on pin 19
*/
bfs::TruStabilityRsc pres(&SPI, 20, 19);

int main() {
  /* Serial monitor for displaying data */
  Serial.begin(115200);
  while (!Serial) {}
  /* Initialize sensor */
  SPI.begin();
  pres.Begin();
  /* Set temperature SRD */
  pres.temp_srd(10);
  while (1) {
    /* Read and display data */
    pres.Read();
    Serial.print(pres.pres_pa());
    Serial.print("\t");
    Serial.println(pres.die_temp_c());
    delay(100);
  }
}

