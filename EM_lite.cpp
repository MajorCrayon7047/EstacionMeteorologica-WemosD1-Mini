#include "EM_lite.hpp"

//constructor
EMlite::EMlite(int8_t dhtPin, int8_t dhtType) : _dht(dhtPin, (dhtType==11 ? DHT11 : DHT22)){
    //DECLARACION Y INICIALIZACION DE SENSORES
    _dht.begin();
    Adafruit_BMP085 bmp_;
    if (!_bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1) {}
    }
}

float* EMlite::get_data(){
    _presion = _bmp.readPressure() * 0.01;
    _t1 = _bmp.readTemperature();
    _t2 = _dht.readTemperature();
    _h = _dht.readHumidity();
    while(isnan(_t2) || isnan(_h)){
        _t2 = _dht.readTemperature();
        _h = _dht.readHumidity();
    }
    float* data = new float[4];
    data[0] = _t1;
    data[1] = _t2;
    data[2] = _h;
    data[3] = _presion;
    
    return data;
}