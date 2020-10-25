#include <Arduino.h>

class TC {

  public:
    TC(int8_t sck, int8_t miso, int8_t cs);
    float get_temperature();  
    float T = 0;
    
  private:
    int8_t SCK, MISO, CS;
    uint8_t spi_transfer();

};

