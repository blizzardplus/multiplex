#include "Util_converter.h"

#include <iostream>

void Util_converter::convert_int_to_bitStream(unsigned int n, unsigned char* buf)
{
	buf[3] = (n >> 24) & 0xFF;
	buf[2] = (n >> 16) & 0xFF;
	buf[1] = (n >> 8) & 0xFF;
	buf[0] = n & 0xFF;
}


int Util_converter::convert_bit_Stream_to_int(unsigned char* buf)
{
	int n;
	n = *(int *) buf;
	return n;
}
