#ifndef RTC_H
#define RTC_H

#include <stdint.h>

// TODO: read ACPI FADT to accurately check for century register instead of
// assuming century 20.

struct rtc_time {
	uint8_t sec, min, hour;
	uint8_t day, month;
	uint16_t year;
};

void rtc_init(void);
struct rtc_time rtc_get_time(void);
struct rtc_time rtc_get_boot_time(void);

#endif
