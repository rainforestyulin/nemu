#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *expr);
void free_wp(WP *wp);
void print_wp(){
	WP* temp=head;
	while(temp!=NULL){
		printf("wp_%d_expr_%s\n",temp->NO,temp->expr);
		temp=temp->next;
	}
}
WP* new_wp(char *expr){

	/*if(*head==NULL){
		head=free_;
		free_=free_->next;
		head->next=NULL;
		return head;
	}else{
		if(free_!=NULL){
			WP* temp=head;
			while(head!=NULL){
				head=head->next;
			}
			head=free_;
			head->next=NULL;
			free_=free_->next;
		}else{
			assert(-1);
		}
	}*/
	if(free_!=NULL){
		WP* temp=head;
		while(temp!=NULL){
			temp=temp->next;
		};
		temp=free_;
		int i=0;
		while(*(expr+i)!='\0'){
			temp->expr[i]=*(expr+i);
			i++;
		}
		temp->expr[i]='\0';
		free_=free_->next;
		return temp;

	}else{
		assert(-1);
		return 0;
	}
}
void free_wp(WP *wp){
	WP* temp=head;
	unsigned int index=0;
	while(temp->NO!=wp->NO&&index<=NR_WP){
		temp=temp->next;
		index++;
	}
	assert(index<=NR_WP);
	if(temp==head){
		WP* ttemp=free_;
		free_=temp;
		free_->next=ttemp;
		head=NULL;
	}else{
		temp=head;
		while(index-1>0){
			temp=temp->next;
		}
		temp->next=temp->next->next;
		WP* ttemp=free_;
		free_=temp;
		free_->next=ttemp;
	}
}
