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

void Aggregator::process(unsigned char * in_buf, int buf_len, int subBuf_size)
{
	unsigned char flow_id_bits[4];   // id of each flow that will be splitted by the various subflows
	unsigned int flow_id;

	memcpy(flow_id_bits, in_buf,4);

	flow_id = Util_converter::convert_bit_Stream_to_int(flow_id_bits);
	std::cout << "The flow_id is  " << flow_id << std::endl;


	unsigned char subflow_id_bits[4];
	unsigned int subflow_id; //For each subflow defined identifier

	memcpy(subflow_id_bits, in_buf + 4, 4);
	subflow_id = Util_converter::convert_bit_Stream_to_int(subflow_id_bits);
	std::cout << "The subflow_id is  " << subflow_id << std::endl;

	unsigned char* temp_reg;
	temp_reg = (unsigned char* ) malloc (subBuf_size);
	memcpy(temp_reg, in_buf + 8, subBuf_size);

	std::cout << "The payload is  " << temp_reg << std::endl;

	//TODO : add the temp_reg to the register that keeps the payload of this flow to the position indicated by the subflow
	// possible a huge buffer with mappings
	//have 2-dimensional array, sunexeia allocate kainourgies grammes k free
	//ka8e grammh ena flow
	// san index to flow number, allazei sunexeia t starting point to index(ari8mos sthlhs) einai (current-first)

	//MALLON OXI
	// define an index for the starting point (# of the first flow), and then compute offset with the number of flow (current-first)*


}

void Aggregator::gather(unsigned char** disp_buf, int chunk_size, int flow_num, unsigned char* received_buf, int buf_len)
{

/*
	//unsigned char* flow_id_bits = (unsigned char*) malloc (4);
	unsigned char flow_id_bits[4];   // id of each flow that will be splitted by the various subflows
	unsigned int flow_id;

	flow_id = Util_converter::convert_bit_Stream_to_int(flow_id_bits);
	std::cout << "The flow_id is  " << flow_id << std::endl;


	for (int i=0 ; i<flow_num; i++)
	{

	}




	// read flow_id






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



	int subBuf_size =  buf_len/flow_num;

	memcpy(received_buf,disp_buf[0],subBuf_size);
	memcpy(received_buf+subBuf_size, disp_buf[1], subBuf_size);

	/*
	int i,j;
	int subBuf_size =  buf_len/flow_num;  // assume that it is divided perfectly
	for ( i=0; i<flow_num; i++ )
	{
		for ( j=0; j<subBuf_size; j++ )
		{
			received_buf[ (i*subBuf_size) + j] = disp_buf[i][j];
		}
	}
	*/
}
