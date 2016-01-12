#include "Dispatcher.h"
#include <iostream>
#include "Util_converter.h"

#include <string.h>
#include <stdlib.h>

void Dispatcher::add_header(unsigned char* sent_buf, int buf_len, int  pck_num , unsigned char* mod_buf)
{
	unsigned char pck_num_bits[4];
	Util_converter::convert_int_to_bitStream(pck_num,pck_num_bits);
	unsigned char buf_len_bits[4];
	Util_converter::convert_int_to_bitStream(buf_len,buf_len_bits);
	memcpy(mod_buf, pck_num_bits,4);
	memcpy(mod_buf + 4, buf_len_bits,4);
	memcpy(mod_buf + 8, sent_buf, buf_len);   // ADD actual data that corresponds to the subflow
}

