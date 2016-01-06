class Aggregator {

public:
	//Aggregator();
	void gather(unsigned char** disp_buf, int chunk_size, int flow_num, unsigned char* received_buf, int buf_len);
	void process(unsigned char * in_buf, int buf_len, int subBuf_size);
	static void removeHeader(unsigned char * in_buf, unsigned int *buf_len, unsigned int *pck_num);
	static void appendPayload(unsigned char * in_buf, unsigned char * buf,unsigned int *buf_len);

private:
	unsigned char* received_buf;
};
