#include <lcom/lcf.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

int (writeRtc)(uint8_t loc, uint8_t cmd);

int (readRtc)(uint8_t loc, uint8_t* out);

bool (rtcUpdating)();

bool (rtcBinary)();

uint8_t (toBinary)(uint8_t n);

int (rtcReadHours)(uint8_t* out);

int (rtcReadMinutes)(uint8_t* out);

int (rtcReadSeconds)(uint8_t* out);
