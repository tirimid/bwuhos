#include "rtc.h"

#include "katomic.h"
#include "kutil.h"
#include "port.h"

#define PORT_CMOS_REG 0x70
#define PORT_CMOS_DATA 0x71

// assume this century when better data not available.
#define ASSUME_CENTURY (mode_flags & MF_BIN ? 20 : 0x20)

enum reg {
	R_SEC = 0x0,
	R_MIN = 0x2,
	R_HOUR = 0x4,
	R_WEEKDAY = 0x6,
	R_DAY = 0x7,
	R_MONTH = 0x8,
	R_YEAR = 0x9,
	R_STATUS_A = 0xa,
	R_STATUS_B = 0xb,
};

enum mode_flag {
	MF_BIN = 0x1, // clear if BCD.
	MF_24_HOUR = 0x2, // clear if 12-hour.
};

static uint8_t rd_cmos_reg(uint8_t reg);

static struct rtc_time boot_time;
static uint8_t mode_flags;
static k_mutex_t mutex;

void
rtc_init(void)
{
	ku_println(LT_INFO, "rtc: init");
	
	uint8_t status_b = rd_cmos_reg(R_STATUS_B);
	mode_flags = 0;
	mode_flags |= MF_BIN * !!(status_b & 0x4); // binary mode.
	mode_flags |= MF_24_HOUR * !!(status_b & 0x2); // 24-hour mode.
	
	boot_time = rtc_get_time();
}

struct rtc_time
rtc_get_time(void)
{
	k_mutex_lock(&mutex);
	
	// wait for "in progress" flag to clear.
	while (rd_cmos_reg(R_STATUS_A) & 0x80)
		;
	
	uint8_t sec = rd_cmos_reg(R_SEC);
	uint8_t min = rd_cmos_reg(R_MIN);
	uint8_t hour = rd_cmos_reg(R_HOUR);
	uint8_t day = rd_cmos_reg(R_DAY);
	uint8_t month = rd_cmos_reg(R_MONTH);
	uint16_t year = rd_cmos_reg(R_YEAR);
	uint8_t century = ASSUME_CENTURY;
	
	// convert BCD to binary.
	if (!(mode_flags & MF_BIN)) {
		sec = (sec & 0x0F) + sec / 16 * 10;
		min = (min & 0x0F) + min / 16 * 10;
		hour = (hour & 0x0F) + (hour & 0x70) / 16 * 10 | (hour & 0x80);
		day = (day & 0x0F) + day / 16 * 10;
		month = (month & 0x0F) + month / 16 * 10;
		year = (year & 0x0F) + year / 16 * 10;
		century = (century & 0x0F) + century / 16 * 10;
	}
	
	// convert 12-hour to 24-hour if PM bit set.
	if (!(mode_flags & MF_24_HOUR) && (hour & 0x80))
		hour = ((hour & 0x7f) + 12) % 24;
	
	// convert year to normal calendar format using century.
	year += 100 * century;
	
	struct rtc_time time = {
		.sec = sec,
		.min = min,
		.hour = hour,
		.day = day,
		.month = month,
		.year = year,
	};
	k_mutex_unlock(&mutex);
	return time;
}

struct rtc_time
rtc_get_boot_time(void)
{
	return boot_time;
}

static uint8_t
rd_cmos_reg(uint8_t reg)
{
	port_wr(PORT_CMOS_REG, 0x80 | reg, PS_8); // 0x80 for NMI disable.
	return port_rd(PORT_CMOS_DATA, PS_8);
}
