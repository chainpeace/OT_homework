/*
	 This code is written for the purpose of practicing GDB tools.
	 It is a simple implementation of linked list and examples of using it. 
	 When this code is compiled and run, it will result in Segmentation Fault.
	 Use GDB(GNU project Debugger) 
	 to track down function path and find which line is the cause of Segmentation fault. 
	 Good Luck!
	 -YSCHOI-
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct _node{
	int data;
	struct _node *next;
} node;

void print_list(node *head){
	node *curr = head;
	printf("[");
	while(curr->next != NULL){
		curr = curr->next;
		printf(" %d", curr->data);
	}
	printf(" ]\n");
	return;
}

void insert(node *head, int newData){
	node *newNode = (node *)malloc(sizeof(node));
	newNode->data = newData;
	newNode->next = NULL;

	node *curr = head;

	while(curr->next != NULL){
		curr = curr->next; 
	}
	curr->next = newNode;

	return;
}

int search(node *head, int searchData){
	int loc = 0;
	int found = 0;
	node *curr = head;
	
	while(found != 1 && curr->next != NULL){
		curr = curr->next;
		loc++;
		if(curr->data == searchData)
			found = 1;
	}
	return loc; 
}

void change(node *head, int loc, int changeData){
	node *curr = head;
	while(loc > 0){
		curr = curr->next;
		loc--;
	}
	curr->data = changeData;
	return;
}


void delete(node *head, int loc){
	
	node *curr = head;

	while(loc > 1){
		curr = curr->next;
		loc--;
	}
	node *before = curr;
	node *tmp = before->next;
	before->next = before->next->next;
	free(tmp);
	/*
	while(loc > -1){
		curr = curr->next;
		loc--;
	}
	node *after = curr;

	free(before->next);
	before->next = after;
*/
	return;

}
void incr_list(node* head, int val){
	node *curr = head;
	int loc = 0;
	while(curr->next != NULL){
		curr=curr->next;
		loc++;
		change(head, loc, (curr->data) + val);
	}
	return;
}

void multiply_list(node* head, int val){
	node *curr = head;
	int loc = 0;
	while(curr->next != NULL){
		curr=curr->next;
		loc++;
		change(head, loc, (curr->data) * val);
	}
	return;

}

void delete_by_mod_n_cond(node* head, int n, int val){
	node *curr = head;
	int loc = 1;
	while(curr != NULL){
		curr=curr->next;
		if(curr->data % n == val)
			delete(head, loc);
		else
			loc++;
	}
}

void mutate_list(node* head){
	//multiply by 2.
	multiply_list(head, 2);	
	//print_list(head);
	
	//add 1
	incr_list(head, 1);
	//print_list(head);

	//delete (mod 3) = 0
	delete_by_mod_n_cond(head, 3, 0);
	//print_list(head);

	//add 2
	incr_list(head, 2);
	//print_list(head);

	//delete (mod 4) = 3
	delete_by_mod_n_cond(head, 4, 3);
	//print_list(head);
}

int main(){
	
	node *head = (node *)malloc(sizeof(node));
	head->next = NULL;
	for(int i=0; i<10; i++)
		insert(head, i);
	//print_list(head);
	mutate_list(head);
	print_list(head);
	return 0;
}
