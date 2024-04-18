/*
     Please see COPYRIGHT AND LICENSE information at the end of this file.
  
     ----- 
  
     codect-API.c
  
     $Id$
*/

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "codecti.h"
#include "codect_proto.h"
#include "codect-API.h"

void codect_init() {
    char *codect_env;

    codect_env = getenv ("CODECT");
    if (codect_env != NULL && strstr (codect_env, "-g") != NULL)
        codect_debug = 1;
    else
        codect_debug = 0;

    codecti_init();
    return;
}

void codect_record_callsite(codect_pc_cs_t *cs) {
    codecti_record_callsite(cs);
    return;
}

void codect_resolve_callsite(codect_pc_cs_t *cs) {
    codecti_resolve_callsite(cs);
    return;
}

void codect_print_callsite(codect_src_cs_t *cs) {
    codecti_print_callsite(cs);
    return;
}

void codect_pc_cs_export(struct callsite_stats cs, void **start_p, size_t *len) {
    codecti_serialize_callsite(cs, start_p, len);
    return;
}

void codect_src_cs_import(void *start_p, size_t len, struct callsite_stats *cs) {
    codecti_deserialize_callsite(start_p, len, cs);
    return;
}


void codect_pc_ht_init(codect_pc_ht_t **ht) {
    codecti_pc_cs_cache_init(ht);
}

void codect_src_ht_init(codect_src_ht_t **ht) {
    codecti_src_cs_cache_init(ht);
}

void codect_copy_pc_cs(codect_pc_cs_t *new_cs, codect_pc_cs_t pc_cs) {
    codecti_copy_pc_cs(new_cs, pc_cs);
}

void codect_copy_src_cs(codect_pc_cs_t *new_cs, codect_pc_cs_t src_cs) {
    codecti_copy_src_cs(new_cs, src_cs);
}

codect_pc_cs_t *codect_pc_ht_search(codect_pc_ht_t *ht, codect_pc_cs_t pc_cs) {
    return codecti_ht_search_pc_cache(ht, pc_cs);
}

codect_src_cs_t *codect_src_ht_search(codect_src_ht_t *ht, codect_src_cs_t src_cs) {
    return codecti_ht_search_src_cache(ht, src_cs);
}

void codect_pc_ht_insert(codect_pc_ht_t *ht, codect_pc_cs_t *pc_cs) {
    codecti_ht_insert_cache(ht, pc_cs);
}

void codect_src_ht_insert(codect_src_ht_t *ht, codect_src_cs_t *src_cs) {
    codecti_ht_insert_cache(ht, src_cs);
}

void codect_gather_pc_ht_data(codect_pc_ht_t *ht, int *count, void ***data) {
    codecti_ht_gather_cache_data(ht, count, data);
}

void codect_gather_src_ht_data(codect_src_ht_t *ht, int *count, void ***data) {
    codecti_ht_gather_cache_data(ht, count, data);
}

void codect_pc_ht_free(codect_pc_ht_t *ht) {
    codecti_free_pc_cache(ht);
}

void codect_src_ht_free(codect_pc_ht_t *ht) {
    codecti_free_src_cache(ht);
}

void codect_fini() {
    codecti_fini ();
    return;
}