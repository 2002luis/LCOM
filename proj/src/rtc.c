#include <lcom/lcf.h>

#include "rtc.h"

int (readRtc)(uint8_t loc, uint8_t* out){
    if(sys_outb(RTC_ADDR_REG, loc)) return 1;
    return util_sys_inb(RTC_DATA_REG, out);
}

bool (rtcUpdating)(){
    uint8_t out = 0;
    if (readRtc(10, &out)) return 1;
	return out & BIT(7);
}

bool (rtcBinary)(){
    uint8_t out = 0;
    if (readRtc(11, &out)) return 1;
	return out & BIT(2);
}

int (rtcReadHours)(uint8_t* out){
    if (rtcUpdating()) return 1;
    return(readRtc(4, out));
}
