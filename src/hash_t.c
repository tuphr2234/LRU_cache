#include <stdlib.h> 	/* malloc ,size_t*/
#include <assert.h>		/* assert		*/
#include <stdio.h>		/*printf		*/


#include "dlist.h" 		/* dlist_t */

#include "aux_funcs.h" /*is_match_t , action_func*/
#include "hash_t.h"

#define UNUSED(x) ((void)(x))

struct hash
{
	hash_func_t hash_func;
	dlist_t **table;
	size_t table_size;
	is_match_func_t match;
};


typedef struct hash_elem
{
	const void *key;
	void *val;
}hash_elem_t;

hash_elem_t null_elem = {NULL,NULL};



typedef struct user_params
{
	is_match_func_t match;
	void *key;

}user_params_t;

typedef struct user_params_action
{
	action_func_t action_func;
	void *val;

}user_params_action_t;


static int isMatch(const void *data, const void *user_params)
{
	hash_elem_t *data_elem = (hash_elem_t*)data;
	user_params_t *up = (user_params_t*)user_params;

	return up->match(data_elem->key, up->key);
}

static int ActionOnVal( void *data,  void *user_params)
{
	hash_elem_t *data_elem = (hash_elem_t*)data;
	user_params_action_t *up = (user_params_action_t*)user_params;
	return up->action_func(data_elem->val , up->val);
}


/******************************************************************************
Description:     	Creates hash table ordered according to "hash_func".
Return value:    	Pointer to hash table in case of success, otherwise NULL.
Time Complexity: 	O(size).
Note:            	Should call "HashDestroy()" at end of use
                    'table_size should be big enough to store hash_func values
******************************************************************************/
hash_t *HashCreate(size_t table_size, hash_func_t hash_func, is_match_func_t match)
{
	hash_t *hash_table = (hash_t*)malloc(sizeof(hash_t));
	size_t i = 0 , j = 0;
	if(NULL == hash_table)
	{
		return NULL;
	}
	hash_table->hash_func = hash_func;
	hash_table->table_size = table_size;
	hash_table->table = (dlist_t**)malloc(table_size * sizeof(dlist_t*));
	hash_table->match = match;
	
	if(NULL == hash_table->table)
	{
		free(hash_table);
		return NULL;
	}
	
	for(i = 0 ; i < table_size ; i++)
	{
		(hash_table->table)[i] = DListCreate();
		
		if(NULL == (hash_table->table)[i])
		{
			assert((hash_table->table)[i]);
			for(j = 0 ; j < i ; j++)
			{
				free((hash_table->table)[j]);(hash_table->table)[j] = NULL;
			}
		
			free(hash_table->table);
			free(hash_table);
			return NULL;
		}

	}
	
	return hash_table;
}

static int freeData(void *data, void *user_params)
{
	UNUSED(user_params);

	free((hash_elem_t*)data);

	return 0;
}

static int StaticHashForEach(hash_t *hash, action_func_t action_func, void *user_params)
{
	size_t size = 0;
	size_t i = 0;
	dlist_t *list = NULL;
	
	assert(hash);

	for(i = 0 ; i < hash->table_size ; i++)
	{
		list = hash->table[i];
		
		if(!DListIsEmpty(list))
		{
			ditr_t begin = DListIterBegin(list);
			ditr_t end = DListIterEnd(list);
			DListForEach(begin, end, action_func,user_params);
		}
	}
	
	return size;
}

/******************************************************************************
Description:     	Deletes a hash table pointed to by "hash" from memory.
Time Complexity: 	O(n). 
Notes:           	Undefined behaviour if hash is NULL.
*******************************************************************************/
void HashDestroy(hash_t *hash)
{
	size_t i = 0;
	
	assert(hash);
	
	StaticHashForEach(hash, freeData, NULL);
	
	for(i = 0 ; i < hash->table_size ; i++)
	{
		DListDestroy(hash->table[i]);
	}
	
	
	free(hash->table);hash->table=NULL;
	free(hash);hash=NULL;
}


/*******************************************************************************
Description:     	Deletes the element related to 'key' from hash table.
Time Complexity: 	O(1) average, O(n) worst case.
Notes               Undefined behaviour if hash is invalid pointer.
					undefined behaviour if key is invalid pointer.
*******************************************************************************/
void HashRemove(hash_t *hash, const void *key)
{

	assert(hash);
	assert(key);
	{
	hash_elem_t *found = NULL;
	ditr_t find;
	size_t index = hash->hash_func(key);
	dlist_t *list = hash->table[index];
	ditr_t begin = DListIterBegin(list);
	ditr_t end = DListIterEnd(list);
	user_params_t up = {0};
	
	up.match = hash->match;
	up.key = (void*)key;

	find = DListFind(begin,end ,isMatch, &up);
	
	if(DListIterIsEqual(end,find))
	{
		return;
	}
	
	found = (hash_elem_t*)DListGetData(find);

	free(found);
	DListRemove(find);
	
	}
	
	
}

/*******************************************************************************
Description:     	Inserts 'val' to the correct place in 'hash' according to 
					'hash_func' provided by user at 'HashCreate' 
Return value:    	0 in case of success otherwise 1.  
Time Complexity: 	O(1) average, O(n) worst case.
Notes: 			 	Undefined behaviour if hash is invalid pointer.
					Undefined behaviour if key is invalid pointer.
*******************************************************************************/
int HashInsert(hash_t *hash,const void *key, void *val)
{
	size_t index = 0;
	hash_elem_t *elem = (hash_elem_t*)malloc(sizeof(hash_elem_t));
	if(NULL == elem)
	{
		return 1;
	}
	elem->key = key;
	elem->val = val;
	
	assert(hash);

	index = hash->hash_func(key) % hash->table_size;
	DListPushFront(hash->table[index],elem);
	
	return 0;
}

/*******************************************************************************
Description:     	Returns number of elements in "hash".
Time Complexity: 	O(n).
Notes:			 	Undefined behaviour if 'hash' is invalid pointer.
*******************************************************************************/
size_t HashSize(const hash_t *hash)
{
	size_t size = 0;
	size_t i = 0;
	
	assert(hash);
	
	for(i = 0 ; i < hash->table_size ; i++)
	{
		size+=DListSize(hash->table[i]);
	}
	
	return size;
}

/*******************************************************************************
Description: 		Checks if "hash" is empty.
Return value:   	1 for true, 0 for false.
Time complexity:  	O(n)
Note: 				Undefined behaviour if 'hash' is invalid pointer.
*******************************************************************************/
int HashIsEmpty(const hash_t *hash)
{
	
	assert(hash);
	return 0 == HashSize(hash);
}


/*******************************************************************************
Description:		Finds 'val' mapped to 'key'.
Return value:       Pointer to val in case of success, otherwise NULL.
Time complexity:    O(1) average, O(n) worst case.
Note:          		Undefined behaviour if 'hash' is invalid pointer.
*******************************************************************************/

void *HashFind(const hash_t *hash, const void *key)
{

	assert(hash);
	{
	hash_elem_t *found = NULL;
	size_t index = hash->hash_func(key);
	dlist_t *list = hash->table[index];
	ditr_t begin = DListIterBegin(list);
	ditr_t end = DListIterEnd(list);

	user_params_t up = {0};
	up.match = hash->match;
	up.key = (void*)key;

	{
	
	ditr_t find = DListFind(begin,end ,isMatch, &up);
	
	if(DListIterIsEqual(end,find))
	{
		return NULL;
	}
	
	found = (hash_elem_t*)DListGetData(find);
	return found->val;

	}

	}
}


/*******************************************************************************
Description:  	  	Calls "action_func" on all elements in hash until fail.
Return value:     	0 for success, 1 for fail.
Time complexity:  	O(n)
Notes:            	Undefined behaviour if 'hash' is invalid pointer.
*******************************************************************************/
int HashForEach(hash_t *hash, action_func_t action_func, void *user_params)
{
	size_t size = 0;
	size_t i = 0;
	dlist_t *list = NULL;
	user_params_action_t up = {0};
	
	up.action_func = action_func;
	up.val = user_params;
	
	assert(hash);

	for(i = 0 ; i < hash->table_size ; i++)
	{
		list = hash->table[i];
		
		if(!DListIsEmpty(list))
		{
			ditr_t begin = DListIterBegin(list);
			ditr_t end = DListIterEnd(list);
			DListForEach(begin, end, ActionOnVal,&up);
		}

	}
	
	return size;
}





