/* -*- C -*-
   Please see COPYRIGHT AND LICENSE information at the end of this file.

   -----

   codecti.c
 */

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <string.h>
#include <float.h>
#include <unistd.h>
#include "codecti.h"

static void
_codecti_init (char *appName)
{
  codecti.appName = strdup (appName);
  codecti.stdout_ = stdout;
  codecti.stderr_ = stderr;

  codecti.baseNames = 1;
  codecti.inAPIrtb = 0;
  codecti.do_lookup = 1;

  codecti.tableSize = 256;
  codecti.reportStackDepth = 32;

  codecti.internalStackDepth = CODECT_INTERNAL_STACK_DEPTH;
  codecti.fullStackDepth = codecti.reportStackDepth + codecti.internalStackDepth;
  if ( codecti.fullStackDepth > CODECT_CALLSITE_STACK_DEPTH_MAX )
      codecti.fullStackDepth = CODECT_CALLSITE_STACK_DEPTH_MAX;
  codecti.use_pc_cache = 1;
#ifdef SO_LOOKUP
  codecti.so_info = NULL;
#endif
  codecti_msg_debug ("appName is %s\n", appName);
  return;
}

void
codecti_init(char **argv) {
  codecti.toolname = "CodeCT";
#if defined(Linux) && ! defined(ppc64)
  codecti.appFullName = codecti_get_proc_exe_link ();
  codecti_msg_debug ("appFullName is %s\n", codecti.appFullName);
  _codecti_init (codecti_get_base_app_name (codecti.appFullName));
#else
  if (argv != NULL && *argv != NULL && **argv != NULL)
    {
      _codecti_init (codecti_get_base_app_name (**argv));
      codecti.appFullName = strdup (**argv);
    }
  else
    {
      _codecti_init ("Unknown");
      codecti_msg_debug ("argv is NULL\n");
    }
#endif
  codecti_cs_cache_init();
}

void
codecti_record_callsite (struct callsite_stats **p) {
  int ret = 0;
  struct callsite_stats *call_stat;
  jmp_buf jb;

  call_stat = malloc(sizeof(struct callsite_stats));
  if (NULL == call_stat) {
    ret = 1;
    goto error;
  }
  bzero(call_stat, sizeof(struct callsite_stats));

  setjmp (jb);
  codecti.inAPIrtb = 1;		/*  Used to correctly identify caller FP  */

  ret = codecti_RecordTraceBack (jb, call_stat->pc, codecti.fullStackDepth);
  codecti.inAPIrtb = 0;

  /* here pc traceback is recorded - set a flag to search and push in pc cache */
  codecti.use_pc_cache = 1;
error:
  *p = call_stat;
  // TODO: error handling
  return;
}

void
codecti_resolve_callsite (struct callsite_stats *p) {
#ifdef ENABLE_BFD
  if (codecti.appFullName != NULL)
    {
      if (open_bfd_executable (codecti.appFullName) == 0)
        codecti.do_lookup = 0;
    }
#elif defined(USE_LIBDWARF)
  if (codecti.appFullName != NULL)
    {
      if (open_dwarf_executable (codecti.appFullName) == 0)
        codecti.do_lookup = 0;
    }
#endif
#if defined(ENABLE_BFD) || defined(USE_LIBDWARF)
  else
    {
      codecti_msg_warn
          ("Failed to open executable.\n");
      codecti.do_lookup = 0;
    }
#endif

  codecti_query_src (p);

#ifdef ENABLE_BFD
  if (codecti.appFullName != NULL)
    {
      close_bfd_executable();
      codecti.do_lookup = 0;
    }
#elif defined(USE_LIBDWARF)
  if (codecti.appFullName != NULL)
    {
      close_dwarf_executable ();
      codecti.do_lookup = 0;
    }
#endif
  codecti.use_pc_cache = 0;

  return;
}

void
codecti_print_callsite (struct callsite_stats *p) {
  codecti_profile_print (codecti.stderr_, p);
  return;
}

void
codecti_free_callsite (struct callsite_stats *p) {
  free(p);
  return;
}

int
codecti_ht_insert_callsite (struct callsite_stats *p) {
  if (codecti.use_pc_cache) {
    return codecti_ht_insert_cs_src_id_cache(p);
  } else {
    return codecti_ht_insert_cs_pc_cache(p);
  }
}

void
codecti_serialize_callsite (struct callsite_stats *p, void **start_p, size_t *len) {
  size_t serialized_data_sz = sizeof(struct callsite_stats);
  void *serialized_data = (void *)malloc(serialized_data_sz); // Do we need this

  memcpy(serialized_data, p, serialized_data_sz);
  
  *start_p = serialized_data;
  *len = serialized_data_sz;
  return;
}

void
codecti_deserialize_callsite (void *start_p, size_t len, struct callsite_stats **p) {
  void *cs = NULL;

  assert (len == sizeof(struct callsite_stats));

  cs = (void *)malloc(sizeof(struct callsite_stats));
  memcpy(cs, start_p, len);
  free(start_p);

  *p = (struct callsite_stats *)cs;
  return;
}

void
codecti_fini () {
  codecti_free_pc_cache();
  codecti_free_src_id_cache();
  codecti_cs_cache_fini();
  return;
}

/*

<license>

Copyright (c) 2006, The Regents of the University of California.
Produced at the Lawrence Livermore National Laboratory
Written by Jeffery Vetter and Christopher Chambreau.
UCRL-CODE-223450.
All rights reserved.

This file is part of mpiP.  For details, see http://llnl.github.io/mpiP.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the disclaimer below.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the disclaimer (as noted below) in
the documentation and/or other materials provided with the
distribution.

* Neither the name of the UC/LLNL nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OF
THE UNIVERSITY OF CALIFORNIA, THE U.S. DEPARTMENT OF ENERGY OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Additional BSD Notice

1. This notice is required to be provided under our contract with the
U.S. Department of Energy (DOE).  This work was produced at the
University of California, Lawrence Livermore National Laboratory under
Contract No. W-7405-ENG-48 with the DOE.

2. Neither the United States Government nor the University of
California nor any of their employees, makes any warranty, express or
implied, or assumes any liability or responsibility for the accuracy,
completeness, or usefulness of any information, apparatus, product, or
process disclosed, or represents that its use would not infringe
privately-owned rights.

3.  Also, reference herein to any specific commercial products,
process, or services by trade name, trademark, manufacturer or
otherwise does not necessarily constitute or imply its endorsement,
recommendation, or favoring by the United States Government or the
University of California.  The views and opinions of authors expressed
herein do not necessarily state or reflect those of the United States
Government or the University of California, and shall not be used for
advertising or product endorsement purposes.

</license>

*/


/* eof */
