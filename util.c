/* -*- C -*- 

   mpiP MPI Profiler ( http://llnl.github.io/mpiP )

   Please see COPYRIGHT AND LICENSE information at the end of this file.

   -----

   util.c -- misc util functions

 */

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "mpiPi.h"

char *
GetBaseAppName (char *rawName)
{
  char *cp;

  if (rawName == NULL)
    return strdup ("Unknown");

  /* delete directorys and make a copy, if nec */
  if ((cp = rindex (rawName, '/')) == NULL)
    {
      return rawName;
    }
  else
    {
      return cp + 1;		/* skip over the '/' */
    }
}

#ifdef Linux
char *
getProcExeLink ()
{
  int pid, exelen, insize = 256;
  char *inbuf = NULL, file[256];

  pid = getpid ();
  snprintf (file, 256, "/proc/%d/exe", pid);
  inbuf = malloc (insize);
  if (inbuf == NULL)
    {
      mpiPi_abort ("unable to allocate space for full executable path.\n");
    }

  exelen = readlink (file, inbuf, 256);
  if (exelen == -1)
    {
      if (errno != ENOENT)
        {
          while (exelen == -1 && errno == ENAMETOOLONG)
            {
              insize += 256;
              inbuf = realloc (inbuf, insize);
              exelen = readlink (file, inbuf, insize);
            }
          inbuf[exelen] = '\0';
          return inbuf;
        }
      else
        free (inbuf);
    }
  else
    {
      inbuf[exelen] = '\0';
      return inbuf;
    }
  return NULL;
}
#endif

char *
mpiP_format_address (void *pval, char *addr_buf)
{
  static int get_sys_info = 0;
  static int ptr_hex_chars = 0;
  static char hex_prefix[3] = "";
  char test_buf[8] = "";

  if (get_sys_info == 0)
    {
      ptr_hex_chars = sizeof (char *) * 2;
#ifdef Linux
      snprintf (test_buf, 8, "%p", (void *) 0x1);
#else
      snprintf (test_buf, 8, "%0p", (void *) 0x1);
#endif

      if (strcmp (test_buf, "0x1") != 0)
        strcpy (hex_prefix, "0x");

      get_sys_info = 1;
    }

#ifdef Linux
  sprintf (addr_buf, "%s%p", hex_prefix, pval);
#else
  sprintf (addr_buf, "%s%0.*p", hex_prefix, ptr_hex_chars, pval);
#endif

  return addr_buf;
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
