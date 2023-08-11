#ifndef EM_lite
#define EM_lite
#include "Arduino.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

class EMlite {
    public:
        EMlite(int8_t dhtPin, int8_t dhtType);
        float* get_data();
    private:
        DHT _dht;
        Adafruit_BMP085 _bmp;
        float _t1, _t2, _h, _presion;
};
#endif