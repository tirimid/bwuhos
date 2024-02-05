#include "kutil.h"

#include "dev/serial_port.h"

void
ku_log(enum log_type type, char const *msg, ...)
{
	switch (type) {
	case LT_INFO:
		sp_write_str("[info] ");
		break;
	case LT_ERR:
		sp_write_str("[err] ");
		break;
	}
	
	sp_write_str(msg);
	sp_write_ch('\n');
}
