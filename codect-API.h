/*
     Please see COPYRIGHT AND LICENSE information at the end of this file.
  
     ----- 
  
     codect-API.h
*/

#ifndef _CODECTAPI_H
#define _CODECTAPI_H

typedef struct callsite_stats *codect_callsite_stats_h;

void codect_init(char **argv); //TODO: do not pass argv, always resolve in Linux way
void codect_record_callsite(codect_callsite_stats_h *cs_h);
void codect_resolve_callsite(codect_callsite_stats_h cs_h);
void codect_print_callsite(codect_callsite_stats_h cs_h);
// Insert an element in hash-table, return 1/0 if not found(inserted)/found(not inserted)
int codect_ht_insert_callsite(codect_callsite_stats_h cs_h);
void codect_serialize_callsite(codect_callsite_stats_h cs_h, void **start_p, size_t *len);
void codect_deserialize_callsite(void *start_p, size_t len, codect_callsite_stats_h *cs_h);
void codect_free_callsite(codect_callsite_stats_h cs_h);
void codect_fini();

#endif