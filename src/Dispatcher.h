class Dispatcher {

public:
	//Dispatcher();
	void dispatch(unsigned char* sent_buf, int buf_len, int chunk_size, int flow_num, unsigned char** disp_buf);
	static void add_header(unsigned char* sent_buf, int buf_len, int  pck_num , unsigned char* mod_buf);
};
