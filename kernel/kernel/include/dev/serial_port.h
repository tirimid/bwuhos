#ifndef DEV_SERIAL_PORT_H
#define DEV_SERIAL_PORT_H

int sp_init(void);
char sp_read_ch(void);
void sp_write_ch(char ch);
void sp_write_str(char const *s);

#endif
