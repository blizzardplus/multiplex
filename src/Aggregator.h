class Aggregator {

public:
	//Aggregator();
	static void removeHeader(unsigned char * in_buf, unsigned int *buf_len, unsigned int *pck_num);
	static void appendPayload(unsigned char * in_buf, unsigned char * buf,unsigned int *buf_len);

private:
	unsigned char* received_buf;
};
