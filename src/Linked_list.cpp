#include <stddef.h>
#include "Linked_list.h"

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>


void Linked_list::insertNode(unsigned char* buf, int buf_len,int pck_num)
{
	  struct node *tmp;
	  tmp = head;

	  struct node* new_node = (struct node*) malloc(sizeof(struct node));

	  new_node->buf = (unsigned char*) malloc(buf_len);
	  memcpy(new_node->buf, buf, buf_len);

	  new_node->buf_len  = buf_len;
	  new_node->pck_num =  pck_num;

	  if(tmp==NULL || ((tmp->pck_num)>=pck_num))
	  {
		  new_node->next = head;
		  head = new_node;
	  }
	  else
	  {
		 // tmp = head;
		  while (tmp->next!=NULL && (tmp->next->pck_num < new_node->pck_num))
		  {
			  tmp = tmp->next;
		  }
		  new_node->next = tmp->next;
		  tmp->next = new_node;
	  }

}


void Linked_list::removeNode()   //remove from the head
{
	struct node* tmp;
	tmp = head;
	if (tmp!=NULL)
	{
		head=head->next;
		free(tmp);
		head_pck ++;
	}
}

int Linked_list::getBufLen()   // of the head
{
	return head->buf_len;
}

unsigned char* Linked_list::getBuf()   // of the head
{
	return head->buf;
}

int Linked_list::check_head()
{
	int i;
	if (head!=NULL)
	{
		i = head->pck_num;
		if (i==head_pck)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}
