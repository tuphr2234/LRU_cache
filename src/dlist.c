#include <stdio.h> /*printf , NULL	*/
#include <stdlib.h>/*malloc , size_t*/
#include <assert.h>/*	assert		*/

#include "dlist.h"

#define ITER_TO_NODE(iter) ((dnode_t*)iter)
#define NODE_TO_ITER(node) ((ditr_t)node)  
#define UNUSED(x) (void)(x)
#define NODE (dnode_t)0;
static int Count(void *data , void *params);
 
struct dnode 
{
	void *data;
	struct dnode *next;
	struct dnode *prev;
};

struct dlist
{
	dnode_t head;
    dnode_t tail;
};



static int Count(void *data , void *params)
{
	int *int_params = (int*)params;
	UNUSED(data);
	(*int_params)++;
	
	return  1;
}


dlist_t *DListCreate(void)
{
	dlist_t *list;
	dnode_t *dummy_begin = NULL;
	dnode_t *dummy_end = NULL;
	
	list = (dlist_t*)malloc(sizeof(dlist_t));
	
	if(NULL == list)
	{	
		return NULL;
	}
	
	dummy_end = &(list->tail);
	dummy_begin = &(list->head);
	dummy_end->data = NULL;
	dummy_begin->data = NULL;
	
	dummy_begin->prev = NULL;
	dummy_begin->next = dummy_end;
	dummy_end->prev = dummy_begin;
	

	dummy_end->data = list;
	
	return list;
}



/*******************************************************************************
description:     	deletes a list pointed by "list" from memory.
Time Complexity: 	determined by system call complexity. 
Notes:           	undefined behaviour if list is NULL.
*******************************************************************************/

void DListDestroy(dlist_t *list)
{

	dnode_t *head = NULL;
	dnode_t *next = NULL;
	assert(list);
	
	head = (&(list->head))->next;
	
	while( !DListIterIsEqual(head , ITER_TO_NODE(DListIterEnd(list)) ))
	{
		next = head->next;
		free(head);
		head = next;
	}
	
	free(list);
	
	list = NULL;
}


/*******************************************************************************
description:     	insert 'data' before 'where' in list pointed by 'list'.
return value:    	iterator to inserted data in success,
                 	otherwise DListIterEnd();            
Time Complexity: 	O(1).
Notes:           	undefined behaviour if 'where' is out of lists range
				 	undefined behaviour of list is invalid pointer.
*******************************************************************************/

ditr_t DListInsertBefore(dlist_t *list , ditr_t where, void *data)
{
	dnode_t *new_where = ITER_TO_NODE(where);
	dnode_t *new_item = NULL;
	
	assert(list);
	assert(where);
	
	new_item = (dnode_t*)malloc(sizeof(dnode_t));

	if(NULL == new_item)
	{
		return NODE_TO_ITER(&(list->tail));
	}
	
	new_item->data = data;
	assert(new_where);
	(new_where->prev)->next = new_item;
	new_item->prev = new_where->prev;
	new_item->next = new_where;
	new_where->prev = new_item;
	
	return NODE_TO_ITER(new_item);
}

/*******************************************************************************
description:     	insert 'data' to end of  list pointed by 'list'.
return value:    	iterator to inserted data in success,
                 	otherwise DListIterEnd();            
Time Complexity: 	O(1) + system call complexity
Notes:        
				 	undefined behaviour of list is invalid pointer.
*******************************************************************************/
ditr_t DListPushBack(dlist_t *list, void *data)
{
	assert(list);
	return DListInsertBefore(list , DListIterEnd(list), data);
}


/*******************************************************************************
description:     	insert 'data' to head of  list pointed by 'list'.
return value:    	iterator to inserted data in success,
                 	otherwise DListIterEnd();            
Time Complexity: 	O(1) + system call complexity
Notes:        
				 	undefined behaviour of list is invalid pointer.
*******************************************************************************/
ditr_t DListPushFront(dlist_t *list, void *data)
{
	assert(list);
	
	return DListInsertBefore(list , DListIterBegin(list), data);
}


/*******************************************************************************
description:     deletes the element in list pointed by "what".
return value:    iterator to next element
Time Complexity: O(1) + system compexity.
Notes:           undefined behaviour if "what" is out of lists range or
                 if "what" is dummy node (DListEnd()) or (DListBegin())        
*******************************************************************************/
ditr_t DListRemove(ditr_t what)
{

	dnode_t *new_where;
	ditr_t next = NULL;
	
	assert(ITER_TO_NODE(what));
	assert(ITER_TO_NODE(DListIterNext(what)));
	assert(ITER_TO_NODE(DListIterPrev(what)));
	
	new_where = ITER_TO_NODE(what);
	
	next = DListIterNext(what);
	
	(new_where->prev)->next = new_where->next;
	(new_where->next)->prev = new_where->prev;
	
	free(new_where);
	new_where = NULL;
	
	return next;
	
}



/*******************************************************************************
description:     	remove the last element of list.
Time Complexity: 	O(1) + system call complexity
Notes:              undefined behaviour of list is invalid pointer or empty.
*******************************************************************************/
void *DListPopBack(dlist_t *list)
{
	void *data = NULL;
	assert(list);
	
	data = DListGetData(DListIterPrev(DListIterEnd(list)));
 	DListRemove(DListIterPrev(DListIterEnd(list)));
 	
 	return data;
}


/*******************************************************************************
description:     	remove the first element of list.
Time Complexity: 	O(1) + system call complexity
Notes:              undefined behaviour of list is invalid pointer ot empty.
*******************************************************************************/
void *DListPopFront(dlist_t *list)
{
	void *data = NULL;
	assert(list);
	data = DListGetData(DListIterBegin(list));
	DListRemove(DListIterBegin(list));
 	
 	return data;
}



/*******************************************************************************
description:     	returns number of elements in "list"
Time Complexity: 	O(n)
Notes:			 	undefined behaviour of list is invalid pointer
*******************************************************************************/
size_t DListSize(const dlist_t *list)
{
	int counter = 0;
	assert(list);
	
	DListForEach(DListIterBegin(list),DListIterEnd(list),Count,&counter);
	 
	return counter;			 
}

/*******************************************************************************
description:     	return data at 'iter'  
Time Complexity: 	O(1)
Notes:           	undefined behaviour if 'itr' is out of lists range.
*******************************************************************************/
void *DListGetData(ditr_t iter)
{
	dnode_t *node = NULL;
	assert(ITER_TO_NODE(iter));
	node = ITER_TO_NODE(iter);
	
	return node->data;
}


/*******************************************************************************
description:     	sets 'itr' to 'new_data'.
Time Complexity: 	O(1)
Notes:           	undefined behaviour if 'itr' is out of lists range.
*******************************************************************************/
void DListSetData(ditr_t itr, void *new_data)
{
	dnode_t *node = NULL;
	assert(itr);
	assert(new_data);
	node = ITER_TO_NODE(itr);
	node->data = new_data;
}


/*******************************************************************************
description:  		returns next iterator of "itr"  
Time complexity:  	O(1)
Note:    			undefined behaviour if "itr" is Dummy End ( DListIterEnd()).
*******************************************************************************/
ditr_t DListIterNext(ditr_t itr)
{

	dnode_t *node = NULL;
	assert(ITER_TO_NODE(itr));
	node = ITER_TO_NODE(itr);
	node = node->next;
	
	return NODE_TO_ITER(node);
	
}


/*******************************************************************************
Description: 		Checks if "list" is empty.
Return value:   	1 for true, 0 for false.
Time complexity:  	O(1)
Note: 				Undefined behaviour if 'list' is invalid pointer.
*******************************************************************************/
int DListIsEmpty(const dlist_t *list)
{
	assert(list);
	
	return (list->head).next == &(list->tail);

}


/*******************************************************************************
description:  		returns previews iterator of "itr"  
Time complexity:  	O(1)
Note:    			undefined behaviour if "itr" is Dummy End ( DListIterEnd()).
*******************************************************************************/

ditr_t DListIterPrev(ditr_t itr)
{

	dnode_t *node = NULL;
	assert(ITER_TO_NODE(itr));
	
	node = ITER_TO_NODE(itr);
	node = node->prev;
	
	return NODE_TO_ITER(node);
	
}


/*******************************************************************************
description: 		returns iterstor to first valid element in "list"   
Time complexity:  	O(1)
Note:        		Dummy End (DListIterEnd()) in case of empty list.
             		undefined behaviour of list is invalid pointer
*******************************************************************************/
ditr_t DListIterBegin(const dlist_t *list)
{
	assert(list);
	
	return NODE_TO_ITER((&(list->head))->next);

}


/*******************************************************************************
description:   		returns to dummy end ( invalid element) 
Time complexity:  	O(1)
Note: 				undefined behaviour of "list" is invalid pointer
*******************************************************************************/
ditr_t DListIterEnd(const dlist_t *list)
{
	assert(list);

	
	return NODE_TO_ITER(&(list->tail));

}


/*******************************************************************************
description: 		checks if iter1 and iter2 are iterators for the same element
return value:   	1 for true, 0 for false
Time complexity:  	O(1)
*******************************************************************************/
int DListIterIsEqual(ditr_t iter1, ditr_t iter2)
{
	assert(ITER_TO_NODE(iter1));
	assert(ITER_TO_NODE(iter2));
	
	return ITER_TO_NODE(iter1) == ITER_TO_NODE(iter2);

}


/*******************************************************************************
description:  	  	calls "action_func" on each element between [from, to) 
                  	( "to" is exclusive) untill fail
return value:     	0 for success, 1 for fail
Time complexity:  	O(n)
Notes:            	undefined behaviour if "from" or "to" are not valid or not 
				  	belongs to the same list.
*******************************************************************************/
int DListForEach(ditr_t from, ditr_t to, action_func_t action_func,
				 void *user_params)
{
	 int success = 0;	 
	 
	 assert(ITER_TO_NODE(from));
	 assert(ITER_TO_NODE(to));
	 
	
	 while( !DListIterIsEqual(from,to) )
	 {
	 	success = action_func( DListGetData(from),user_params );
	 	from = DListIterNext(from);
	 }
	 
	 return success;
				 
}


/*******************************************************************************
description:    	search for 'data' in 'list'
return value:   	iterator * pointing to 'data' in case found otherwise NULL 
Time complexity:    O(n)
Note:          		undefined behaviour if "from" or "to" are not valid or not 
					belongs to the same list.
*******************************************************************************/
ditr_t DListFind(ditr_t from, ditr_t to, is_match_func_t match_func,
 void *user_params)
{
	 int success = 0;	
	 				
	 assert(from);
	 assert(to);
	 
	 
	 while(!DListIterIsEqual(from,to))
	 {
	 	
	 	success = match_func(DListGetData(from),user_params);
	 	
	 	if(success)
	 	{
	 		return from;
	 	}
	 	
	 	from = DListIterNext(from);
	 }
	 
	 return to;
}




/*******************************************************************************
description:  	  	transfer elements from one list to another from start_pos
return value:     	0 for success, 1 for fail
Time complexity:  	O(n)
Notes:            	undefined behaviour if "from" or "to" are not valid or not 
				  	belongs to the same list.
*******************************************************************************/
ditr_t DListSplice(ditr_t dest_where,  ditr_t src_first, ditr_t src_last)
{
	
	dnode_t *src_first_node = NULL;
	dnode_t *dest_where_node = NULL;
	dnode_t *src_last_node = NULL;
	
	assert(ITER_TO_NODE(dest_where));
	assert(ITER_TO_NODE(src_first));
	assert(ITER_TO_NODE(src_last));
	
	src_first_node = ITER_TO_NODE(src_first);
	dest_where_node = ITER_TO_NODE(dest_where);
	src_last_node = ITER_TO_NODE(src_last);
	
	
	/*disconnect src, and reconnect first->prev and last->next*/	
	(src_first_node->prev)->next = src_last_node->next; 
	(src_last_node->next)->prev = (src_first_node->prev); 
	
	
	/*connect first and where->prev*/	
	(dest_where_node->prev)->next = src_first_node;
	src_first_node->prev = dest_where_node->prev;
	
	
	/*connect last and where*/	
	src_last_node->next = dest_where_node;
	(dest_where_node->prev) = src_last;
	
	return src_last; /*******TODO****************/

}



/*******************************************************************************
Description:		Finds all matches according to "match_func" in the range 
					[from, to) ("to" is exclusive).
Return value:   	0 for success, 1 for fail. 
Time complexity:    O(n) + system call complexity
Note:          		Undefined behaviour if "from" or "to" are not valid or do
					not belong to the same list or if match_func is NULL
                    
*******************************************************************************/
int DListMultiFind(ditr_t from, ditr_t to, 
						is_match_func_t match_func, void *user_params, dlist_t *results_list)
{

	 int success = 0;	
	 int fin_success = 0;	
	 			
	 assert(from);
	 assert(to);
	 
	 while(!DListIterIsEqual(from,to))
	 {
	 	
	 	success = match_func(DListGetData(from),user_params);
	 	
	 	if(success)
	 	{
	 		DListPushBack(results_list , DListGetData(from));
	 		fin_success = 1;
	 	}
	 	
	 	from = DListIterNext(from);
	 }
	 
	 return fin_success;


}						





