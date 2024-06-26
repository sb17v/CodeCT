/* -*- C -*- 
   Please see COPYRIGHT AND LICENSE information at the end of this file.

   -----

   report.c -- reporting functions
   $Id$
*/

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <math.h>
#include <float.h>
#include <search.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "codecti.h"

/*  Structure used to track callsite statistics for the detail
 *  sections of the concise report.
 *  */

static void
codecti_print_callsites (FILE * fp, callsite_stats_t *p)
{
  int i, ac;
  int fileLenMax = 18;
  int funcLenMax = 24;
  int stack_continue_flag;
  char addr_buf[24];

  /*  If stack depth is 0, call sites are really just MPI calls */
  if (codecti.reportStackDepth == 0)
    return;


  /* Find longest file and function names for formatting */
  int j, currlen;
  for (j = 0;
        (j < codecti.fullStackDepth) && (p->filename[j] != NULL);
        j++)
    {
      if (NULL == p->pc[j]) {
        break;
      }
        currlen = strlen (p->filename[j]);
        fileLenMax = currlen > fileLenMax ? currlen : fileLenMax;
        currlen = strlen (p->functname[j]);
        funcLenMax = currlen > funcLenMax ? currlen : funcLenMax;
    }

  fprintf (fp, "%3s %-*s %5s %-*s\n",
           "Lev", fileLenMax, "File/Address", "Line", funcLenMax,
           "Parent_Funct");

  j = 0;
  int frames_printed = 0;

  for (j = 0, stack_continue_flag = 1;
        (frames_printed < codecti.reportStackDepth) && (p->filename[j] != NULL) &&
        stack_continue_flag == 1; j++)
    {
        if (NULL == p->pc[j]) {
          break;
        }
        //  May encounter multiple "codect*" filename frames
        // if ( strncmp(p->functname[j], "codect", strlen("codect")) == 0 )
        //     continue;

      if (p->lineno[j] == 0 &&
          (strcmp (p->filename[j], "[unknown]") == 0 ||
            strcmp (p->functname[j], "[unknown]") == 0))
        {
          fprintf (fp, "%3d %-*s %-*s\n",
                    frames_printed,
                    fileLenMax + 6,
                    codecti_format_address (p->pc[j], addr_buf),
                    funcLenMax,
                    p->functname[j]);
        }
      else
        {
          fprintf (fp, "%3d %-*s %5d %-*s\n",
                    frames_printed,
                    fileLenMax,
                    p->filename[j], p->lineno[j],
                    funcLenMax,
                    p->functname[j]);
        }
      /*  Do not bother printing stack frames above main   */
      if (strcmp (p->functname[j], "main") == 0
          || strcmp (p->functname[j], ".main") == 0
          || strcmp (p->functname[j], "MAIN__") == 0)
        stack_continue_flag = 0;

      ++frames_printed;
    }
}

void
codecti_profile_print (FILE * fp, void *p)
{
  assert (fp);
  assert (p);

  codecti_print_callsites (fp, (callsite_stats_t *) p);
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
