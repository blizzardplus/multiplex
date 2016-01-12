#include "Aggregator.h"
#include "Util_converter.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>

void Aggregator::removeHeader(unsigned char * in_buf, unsigned int *buf_len, unsigned int *pck_num)
{
	unsigned char pck_num_bits[4];
	memcpy(pck_num_bits, in_buf,4);
	*pck_num = Util_converter::convert_bit_Stream_to_int(pck_num_bits);

	unsigned char buf_len_bits[4];
	memcpy(buf_len_bits, in_buf + 4,4);
	*buf_len = Util_converter::convert_bit_Stream_to_int(buf_len_bits);
}


void Aggregator::appendPayload(unsigned char * in_buf, unsigned char * buf,unsigned int *buf_len)
{
	memcpy(buf, in_buf + 8, *buf_len);
}
