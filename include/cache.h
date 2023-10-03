
#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdlib.h>    /* size_t        */

#include "aux_funcs.h" /* action_func_t */


typedef struct cache cache_t;


void *CacheGet(cache_t *cache, void *key);

void CacheSet(cache_t *cache, void *key , void *data);






#endif    /*__CACHE_H__*/
