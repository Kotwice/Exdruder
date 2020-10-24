#include <Arduino.h>

class TC {

  public:
    TC(int8_t SCK, int8_t MISO, int8_t CS);
    float read(void);  

  private:
    int8_t sck, miso, cs;
    uint8_t spiread(void);

};

