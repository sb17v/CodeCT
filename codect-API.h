/**
 * 
 * @copyright <put NVIDIA coopyright here> Copyright (c) 2024
 * 
 */


typedef struct callsite_stats *codect_callsite_stats_h;

void codect_init(char **argv);
void codect_record_callsite(codect_callsite_stats_h *cs_h);
void codect_resolve_callsite(codect_callsite_stats_h cs_h);
void codect_print_callsite(codect_callsite_stats_h cs_h);
void codect_fini();
