#include <stddef.h>


class Linked_list {

	struct node {
	  unsigned char* buf ;
	  int buf_len;
	  int pck_num;
	  struct node *next;
	};

public:
	struct node *head;
	int head_pck;


public:
	void insertNode(unsigned char* buf, int buf_len,int pck_num);
	void removeNode();
	int getBufLen();
	int check_head();
	unsigned char* getBuf();
};
