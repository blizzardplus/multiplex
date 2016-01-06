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



void Dispatcher::dispatch(unsigned char* sent_buf, int buf_len, int chunk_size, int flow_num,  unsigned char** disp_buf)
{

	//ADD CONTROL BITS

	//unsigned char* flow_id_bits = (unsigned char*) malloc (4);
	unsigned char flow_id_bits[4];   // id of each flow that will be splitted by the various subflows
	unsigned int flow_id=0;				//should be taken as input !!!!!!


	flow_id ++;
	Util_converter::convert_int_to_bitStream(flow_id,flow_id_bits);
	std::cout << std::hex << (int)flow_id_bits[0] << ' ' << (int)flow_id_bits[1] << ' '
	        << (int)flow_id_bits[2] << ' ' << (int)flow_id_bits[3] << std::endl;



	//For each subflow define an identifier
	unsigned char subflow_id_bits[4];
	unsigned int subflow_id=0;

	int subBuf_size =  buf_len/flow_num;

	for (int i=0 ; i<flow_num; i++)
	{
		memcpy(disp_buf[i], flow_id_bits,4);   // ADD flow_id to output

		subflow_id ++;
		Util_converter::convert_int_to_bitStream(subflow_id,subflow_id_bits);
		std::cout << std::hex << (int)subflow_id_bits[0] << ' ' << (int)subflow_id_bits[1] << ' '
		        << (int)subflow_id_bits[2] << ' ' << (int)subflow_id_bits[3] << std::endl;
		memcpy(disp_buf[i] + 4, subflow_id_bits,4);  //ADD subFlow_id

		memcpy(disp_buf[i] + 8, sent_buf + (subBuf_size*i), subBuf_size);   // ADD actual data that corresponds to the subflow
	}



	//std::cout << "The initial buffer is " << sent_buf << std::endl;



	//memcpy(disp_buf[0], sent_buf,subBuf_size);
	//memcpy(disp_buf[1], sent_buf+subBuf_size,subBuf_size);

	//std::cout << "The initial buffer is " << disp_buf[0] << std::endl;
	//std::cout << "The initial buffer is " << disp_buf[1] << std::endl;


	/*
	disp_buf1 = (unsigned char*) malloc (subBuf_size+1);
	disp_buf2 = (unsigned char*) malloc (subBuf_size+1);
	memcpy(disp_buf1, sent_buf,subBuf_size);
	memcpy(disp_buf2, sent_buf+subBuf_size,subBuf_size);

	cout << "The initial buffer is " << disp_buf1 << endl;
	cout << "The initial buffer is " << disp_buf2 << endl;

*/

	/*
	int i,j;
	int subBuf_size =  buf_len/flow_num;  // assume that it is divided perfectly
	for ( i=0; i<flow_num; i++ )
	{
		for ( j=0; j<subBuf_size; j++ )
		{
			disp_buf[i][j]=sent_buf[ (i*subBuf_size) +j];
		}
	}
	*/
}
