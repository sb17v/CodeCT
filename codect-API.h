/*
     Please see COPYRIGHT AND LICENSE information at the end of this file.
  
     ----- 
  
     codect-API.h
*/

#ifndef _CODECTAPI_H
#define _CODECTAPI_H

#include "codect-config.h"

/* Callsites */
struct callsite_stats 
{
  void *pc[CODECT_CALLSITE_STACK_DEPTH_MAX];
  char *filename[CODECT_CALLSITE_STACK_DEPTH_MAX];
  char *functname[CODECT_CALLSITE_STACK_DEPTH_MAX];
  int lineno[CODECT_CALLSITE_STACK_DEPTH_MAX];
};

typedef struct callsite_stats codect_pc_cs_t;
typedef struct callsite_stats codect_src_cs_t;

/* Hash table */
typedef struct _h_t codect_pc_ht_t;
typedef struct _h_t codect_src_ht_t;

/* Callsite capture and tracking - fix them */
void codect_init();
void codect_record_callsite(codect_pc_cs_t *cs);
void codect_resolve_callsite(codect_pc_cs_t *cs);
void codect_print_callsite(codect_src_cs_t *cs);
void codect_fini();

/* Serialize & deserialize*/
void codect_pc_cs_export(codect_pc_cs_t pc_cs, void **s_data, size_t *len);
void codect_src_cs_import(void *s_data, size_t len, codect_src_cs_t *src_cs);

/* Hashtable func */
/* PC hashtable */
void codect_pc_ht_init(codect_pc_ht_t **ht);
codect_pc_cs_t *codect_pc_ht_search(codect_pc_ht_t *ht, codect_pc_cs_t pc_cs);
void codect_copy_pc_cs(codect_pc_cs_t *new_cs, codect_pc_cs_t pc_cs);
void codect_pc_ht_insert(codect_pc_ht_t *ht, codect_pc_cs_t *pc_cs);
void codect_gather_pc_ht_data(codect_pc_ht_t *ht, int *count, void ***data);
void codect_pc_ht_free(codect_pc_ht_t *ht);

/* SRC hashtable*/
void codect_src_ht_init(codect_src_ht_t **ht);
codect_src_cs_t *codect_src_ht_search(codect_src_ht_t *ht, codect_src_cs_t src_cs);
void codect_copy_src_cs(codect_src_cs_t *new_cs, codect_src_cs_t src_cs);
void codect_src_ht_insert(codect_src_ht_t *ht, codect_src_cs_t *src_cs);
void codect_gather_src_ht_data(codect_src_ht_t *ht, int *count, void ***data);
void codect_src_ht_free(codect_src_ht_t *ht);

#endif