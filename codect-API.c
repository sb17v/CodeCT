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

void codect_init(char **argv) {
    char *codect_env;

    codect_env = getenv ("CODECT");
    if (codect_env != NULL && strstr (codect_env, "-g") != NULL)
        codect_debug = 1;
    else
        codect_debug = 0;

    codecti_init(argv);
    return;
}

void codect_record_callsite(codect_callsite_stats_h *cs_h_p) {
    codecti_record_callsite((struct callsite_stats **) cs_h_p);
    return;
}

void codect_resolve_callsite(codect_callsite_stats_h cs_h) {
    codecti_resolve_callsite((struct callsite_stats *) cs_h);
    return;
}

void codect_print_callsite(codect_callsite_stats_h cs_h) {
    codecti_print_callsite((struct callsite_stats *) cs_h);
    return;
}

int codect_ht_insert_callsite(codect_callsite_stats_h cs_h) {
    return codecti_ht_insert_callsite((struct callsite_stats *)cs_h);
}

void codect_serialize_callsite(codect_callsite_stats_h cs_h, void **start_p, size_t *len) {
    codecti_serialize_callsite((struct callsite_stats *)cs_h, start_p, len);
    return;
}

void codect_deserialize_callsite(void *start_p, size_t len, codect_callsite_stats_h *cs_h) {
    codecti_deserialize_callsite(start_p, len, (struct callsite_stats **)cs_h);
    return;
}

void codect_free_callsite(codect_callsite_stats_h cs_h) {
    codecti_free_callsite((struct callsite_stats *) cs_h);
    return;
}

void codect_fini() {
    codecti_fini ();
    return;
}