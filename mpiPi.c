/* -*- C -*-

   mpiP MPI Profiler ( http://llnl.github.io/mpiP )

   Please see COPYRIGHT AND LICENSE information at the end of this file.

   -----

   mpiPi.c -- main mpiP internal functions

 */

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <string.h>
#include <float.h>
#include <unistd.h>
#include "mpiPi.h"

static void
mpiPi_init (char *appName)
{
  mpiPi.appName = strdup (appName);
  mpiPi.stdout_ = stdout;
  mpiPi.stderr_ = stderr;

  mpiPi.baseNames = 1;
  mpiPi.inAPIrtb = 0;
  mpiPi.do_lookup = 1;

  mpiPi.reportStackDepth = 32;

  mpiPi.internalStackDepth = MPIP_INTERNAL_STACK_DEPTH;
  mpiPi.fullStackDepth = mpiPi.reportStackDepth + mpiPi.internalStackDepth;
  if ( mpiPi.fullStackDepth > MPIP_CALLSITE_STACK_DEPTH_MAX )
      mpiPi.fullStackDepth = MPIP_CALLSITE_STACK_DEPTH_MAX;

#ifdef SO_LOOKUP
  mpiPi.so_info = NULL;
#endif
  mpiPi_msg_debug ("appName is %s\n", appName);
  return;
}

void
codecti_init(char **argv) {
  mpiPi.toolname = "CodeCT";
#if defined(Linux) && ! defined(ppc64)
  mpiPi.appFullName = getProcExeLink ();
  mpiPi_msg_debug ("appFullName is %s\n", mpiPi.appFullName);
  mpiPi_init (GetBaseAppName (mpiPi.appFullName));
#else
  if (argv != NULL && *argv != NULL && **argv != NULL)
    {
      mpiPi_init (GetBaseAppName (**argv));
      mpiPi.appFullName = strdup (**argv);
    }
  else
    {
      mpiPi_init ("Unknown");
      mpiPi_msg_debug ("argv is NULL\n");
    }
#endif
}

static int
codecti_record_cs(struct callsite_stats **p) {
  int ret = 0;
  struct callsite_stats *call_stat;
  jmp_buf jb;

  call_stat = malloc(sizeof(struct callsite_stats));
  if (NULL == call_stat) {
    ret = 1;
    goto error;
  }

  setjmp (jb);
  mpiPi.inAPIrtb = 1;		/*  Used to correctly identify caller FP  */

  ret = mpiPi_RecordTraceBack (jb, call_stat->pc, mpiPi.fullStackDepth);
  mpiPi.inAPIrtb = 0;

error:
  *p = call_stat;
  return ret;
}

void
codecti_record(void **p) {
  codecti_record_cs ((struct callsite_stats **) p);
}

static void
codecti_resolve_cs(struct callsite_stats *p) {
#ifdef ENABLE_BFD
  if (mpiPi.appFullName != NULL)
    {
      if (open_bfd_executable (mpiPi.appFullName) == 0)
        mpiPi.do_lookup = 0;
    }
#elif defined(USE_LIBDWARF)
  if (mpiPi.appFullName != NULL)
    {
      if (open_dwarf_executable (mpiPi.appFullName) == 0)
        mpiPi.do_lookup = 0;
    }
#endif
#if defined(ENABLE_BFD) || defined(USE_LIBDWARF)
  else
    {
      mpiPi_msg_warn
          ("Failed to open executable.  The mpiP -x runtime flag may address this issue.\n");
      mpiPi.do_lookup = 0;
    }
#endif

  mpiPi_query_src (p);

#ifdef ENABLE_BFD
  if (mpiPi.appFullName != NULL)
    {
      close_bfd_executable();
      mpiPi.do_lookup = 0;
    }
#elif defined(USE_LIBDWARF)
  if (mpiPi.appFullName != NULL)
    {
      close_dwarf_executable ();
      mpiPi.do_lookup = 0;
    }
#endif

  return;
}

void
codecti_resolve (void *p) {
  codecti_resolve_cs ((struct callsite_stats *) p);
}

static void
codecti_print_cs (struct callsite_stats *p) {
  mpiPi_profile_print (mpiPi.stderr_, p);
}

void
codecti_print (void *p) {
  codecti_print_cs ((struct callsite_stats *) p);
}

void
codecti_fini () {
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
