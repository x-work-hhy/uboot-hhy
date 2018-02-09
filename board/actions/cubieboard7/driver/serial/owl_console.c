#include "../../include/owl_common.h"

int owl_console_putc(char c)
{
	if (c == '\n')
		owl_serial_putc('\r');

	return owl_serial_putc(c);
}

int owl_console_getc(void)
{
	return owl_serial_getc();
}

int owl_console_init(int id, int baud)
{
	owl_serial_init();
	return 0;
}

