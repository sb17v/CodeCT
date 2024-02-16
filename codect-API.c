#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mpiPi.h"
#include "codect_proto.h"
#include "codect-API.h"

void codect_init(char **argv) {
    // Init internal variables. resolve and set appname
    char *mpiP_env;

    mpiP_env = getenv ("MPIP");
    if (mpiP_env != NULL && strstr (mpiP_env, "-g") != NULL)
        mpiPi_debug = 1;
    else
        mpiPi_debug = 0;

    codecti_init(argv);
    return;
}

void codect_record_callsite(codect_callsite_stats_h *cs_h_p) {
    // Record the callstack
    codecti_record ((void **) cs_h_p);
    return;
}

void codect_resolve_callsite(codect_callsite_stats_h cs_h) {
    // Resolve callsite
    codecti_resolve ((void *) cs_h);
    return;
}

void codect_print_callsite(codect_callsite_stats_h cs_h) {
    // Print callsite
    codecti_print ((void *) cs_h);
    return;
}

void codect_fini() {
    codecti_fini ();
}