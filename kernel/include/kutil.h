#ifndef KUTIL_H__
#define KUTIL_H__

enum log_type {
	LT_INFO = 0,
	LT_ERR,
};

void ku_hang(void);
void ku_log(enum log_type type, char const *msg, ...);

#endif
