#include "../include/i2c.h"

TwoWire i2cScan = TwoWire(1);

void scan( int kanal )
{
  int nDevices = 0;
  byte error;

  for( int i  = 0; i < 128; i++ )
  {
    if( kanal == 1 )
    {
      Wire.beginTransmission(i);
      error = Wire.endTransmission(true);
    }
    else
    {
      i2cScan.beginTransmission(i);
      error = i2cScan.endTransmission(true);
    }

    if( error == 0 )
    {
        Serial.print("I2C Gerät an Adresse 0x");
        if( i < 16 ) Serial.print("0");
        Serial.println(i, HEX);
        nDevices++;
    }
  }

  if( nDevices == 0 )
  {
    Serial.println("Kein I2C Geraet gefunden");
  }
  else
  {
    Serial.println("Ende");
  }
}
