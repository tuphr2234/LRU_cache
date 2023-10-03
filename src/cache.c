#include <stdlib.h> 	/* malloc ,size_t*/
#include <assert.h>		/* assert		*/
#include <stdio.h>		/*printf		*/
#include <string.h>

#include "aux_funcs.h" /*is_match_t , action_func*/

#include "hash_t.h"
#include "dlist.h" 		/* dlist_t */
#include "cache.h"

enum{
    FACTOR = 2
};


struct cache
{
	hash_t *hash_table;
    dlist_t *linked_list;
    size_t hash_capacity;
    size_t size;
};

typedef struct DataAndItr
{
    void *data;
    ditr_t itr;

}data_and_itr_t;


cache_t *CacheCreate(size_t capacity ,hash_func_t hash_func , is_match_func_t match)
{
    cache_t *cache = (cache_t*)malloc(sizeof(cache_t));
   
    if(NULL == cache)
    {
        return NULL;
    }
    
    cache->hash_table = HashCreate(capacity * FACTOR,hash_func, match);
    cache->linked_list = DListCreate();
    cache->size = 0;
    cache->hash_capacity = capacity * FACTOR;

    return cache;
}


void *CacheGet(cache_t *cache, void *key)
{
    assert(cache);
    assert(key);
    
    if(cache->size == 0)
    {
        return NULL;
    }
    else
    {
        /*contains data and ditr_t iterator*/
        data_and_itr_t *data_and_p = (data_and_itr_t*)HashFind(cache->hash_table,key);

        if(NULL == data_and_p)
        {
            return NULL; /*Cache Miss*/
        }
        
        /*update priority LRU*/
        DListRemove(data_and_p->itr);
        DListPushBack(cache->linked_list, key);
    
        /*return data that matches key*/
        return data_and_p->data;
    }
}

void CacheSet(cache_t *cache, void *key , void *data)
{
    
    data_and_itr_t *data_and_itr = (data_and_itr_t*)malloc(sizeof(data_and_itr_t));

    assert(cache);
    assert(key);
    assert(data);

    data_and_itr->data = data;
    data_and_itr->itr = DListPushBack(cache->linked_list,key);

    if(cache->size == cache->hash_capacity)
    {
        /*Cache Miss*/
        void *removeKey = DListPopFront(cache->linked_list);
        HashRemove(cache->hash_table,removeKey);
    }
    else
    {
        ++cache->size;
    }

    HashInsert(cache->hash_table , key , data_and_itr);
}

void CacheDestroy(cache_t *cache)
{
    HashDestroy(cache->hash_table);
    DListDestroy(cache->linked_list);
    free(cache);cache = NULL;
}


size_t hash_func(const void *key)
{
	char key_val = 0; 
    assert(NULL != key);
    key_val = *((char*)key);
    return (size_t)(key_val - 'a');
}
     arg.val = argv[2];

        semctl(semid,  0, SETVAL, arg);

int match(const void *data, const void *user_params)
{
	return 0 == strcmp((char*)data , (char*)user_params);
}


int main()
{
    /* code */
    char *str_arr[] = {"arr" ,"bow" ,"cow" ,"dog" ,"error", "fog", "glory", "hair" ,"index", "jane",
						"knight", "light", "might", "no","olympic","pop","queue","raise","sort","tatto",
						"uni","vertex","wow","xor","you","zoo"};
    
    cache_t *cache =  CacheCreate(13,hash_func, match);

    int i = 0;
    for (i = 0; i < 26; i++)
    {
       CacheSet(cache, str_arr[i] , str_arr[i]);
    }
    for (i = 0; i < 26; i++)
    {
        printf("%s\n",CacheGet(cache, str_arr[i]));
    }

    printf("last one ---- %s\n",(char*)DListPopFront(cache->linked_list));
    printf("LRU ---- %s\n",(char*)DListPopBack(cache->linked_list));

    CacheSet(cache, str_arr[0] , str_arr[0]);
   
    printf("last one ---- %s\n",(char*)DListPopFront(cache->linked_list));
    printf("LRU ---- %s\n",(char*)DListPopBack(cache->linked_list));

    printf("%s\n",CacheGet(cache, str_arr[10]));

    printf("last one ---- %s\n",(char*)DListPopFront(cache->linked_list));
    printf("LRU ---- %s\n",(char*)DListPopBack(cache->linked_list));

    CacheDestroy(cache);

    return 0;
}
