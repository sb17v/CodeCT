/* -*- C -*-
   Please see COPYRIGHT AND LICENSE information at the end of this file.

   -----

   codect-callsites.c

   $Id$
*/

#ifndef lint
static char *svnid = "$Id$";
#endif

#include <string.h>
#include "codect-callsites.h"
#include "codecti.h"

/*
 * ============================================================================
 *
 * Program counter (PC) to source code id: File/Function/Line
 *
 * ============================================================================
 */

typedef callsite_stats_t callsite_pc_cache_entry_t;
typedef callsite_stats_t callsite_src_id_cache_entry_t;

h_t *callsite_pc_cache = NULL;
h_t *callsite_src_id_cache = NULL;

/* Changed this comparator to compare the enire pc list*/
static int
callsite_pc_cache_comparator (const void *p1, const void *p2)
{
  int i;
  callsite_pc_cache_entry_t *cs1 = (callsite_pc_cache_entry_t *) p1;
  callsite_pc_cache_entry_t *cs2 = (callsite_pc_cache_entry_t *) p2;

  for (i = 0; i < codecti.fullStackDepth; i++)
  {
    if ((long) cs1->pc > (long) cs2->pc)
  	{
  	  return 1;
  	}
    if ((long) cs1->pc < (long) cs2->pc)
  	{
  	  return -1;
  	}
  
  }

  return 0;
}

/* Changed this to take account of entire PC list */
static int
callsite_pc_cache_hashkey (const void *p1)
{
  int i, res = 0;
  callsite_pc_cache_entry_t *cs1 = (callsite_pc_cache_entry_t *) p1;
  for (i = 0; i < codecti.fullStackDepth; i++)
  {
    res ^= ((long) cs1->pc[i]);
  }
  return 662917 ^ res;
}

static int
callsite_src_id_cache_comparator (const void *p1, const void *p2)
{
  int i;
  callsite_src_id_cache_entry_t *csp_1 = (callsite_src_id_cache_entry_t *) p1;
  callsite_src_id_cache_entry_t *csp_2 = (callsite_src_id_cache_entry_t *) p2;

#define express(f) {if ((csp_1->f) > (csp_2->f)) {return 1;} if ((csp_1->f) < (csp_2->f)) {return -1;}}
  if (codecti.reportStackDepth == 0)
    {
      return 0;
    }
  else
    {
      for (i = 0; i < codecti.fullStackDepth; i++)
        {
          if (csp_1->filename[i] != NULL && csp_2->filename[i] != NULL)
            {
              if (strcmp (csp_1->filename[i], csp_2->filename[i]) > 0)
                {
                  return 1;
                }
              if (strcmp (csp_1->filename[i], csp_2->filename[i]) < 0)
                {
                  return -1;
                }
              express (lineno[i]);
              if (strcmp (csp_1->functname[i], csp_2->functname[i]) > 0)
                {
                  return 1;
                }
              if (strcmp (csp_1->functname[i], csp_2->functname[i]) < 0)
                {
                  return -1;
                }
            }

          express (pc[i]);
        }
    }
#undef express
  return 0;
}

static int
callsite_src_id_cache_hashkey (const void *p1)
{
  int i, j;
  int res = 0;
  callsite_src_id_cache_entry_t *cs1 = (callsite_src_id_cache_entry_t *) p1;
  for (i = 0; i < codecti.fullStackDepth; i++)
    {
      if (cs1->filename[i] != NULL)
        {
          for (j = 0; cs1->filename[i][j] != '\0'; j++)
            {
              res ^= (unsigned) cs1->filename[i][j];
            }
          for (j = 0; cs1->functname[i][j] != '\0'; j++)
            {
              res ^= (unsigned) cs1->functname[i][j];
            }
        }
      res ^= cs1->lineno[i];
    }
  return 662917 ^ res;
}

void codecti_cs_cache_init()
{
  if (callsite_pc_cache == NULL)
    {
      callsite_pc_cache = h_open (codecti.tableSize,
                                  callsite_pc_cache_hashkey,
                                  callsite_pc_cache_comparator);
    }
  if (callsite_src_id_cache == NULL)
    {
      callsite_src_id_cache = h_open (codecti.tableSize,
                                      callsite_src_id_cache_hashkey,
                                      callsite_src_id_cache_comparator);
    }
}

int
codecti_ht_insert_cs_pc_cache (struct callsite_stats *p) {
  callsite_pc_cache_entry_t key;
  callsite_pc_cache_entry_t *csp;
  int i;

  // TODO: we just need pc list for pc cache
  // Check if initial implementation is working and then fix it
  for (i = 0; i < codecti.fullStackDepth; i++) {
    key.filename[i] = p->filename[i];
    key.functname[i] = p->functname[i];
    key.lineno[i] = p->lineno[i];
    key.pc[i] = p->pc[i]; // Only this field is relevent here
  }

  if (h_search (callsite_pc_cache, &key, (void **) &csp) == NULL) {
    /* key not found */
    /* Allocate a new csp */
    csp =
        (callsite_pc_cache_entry_t *)
        malloc (sizeof (callsite_pc_cache_entry_t));
    bzero (csp, sizeof (callsite_pc_cache_entry_t));

    for (i = 0; i < codecti.fullStackDepth; i++) {
      csp->filename[i] = strdup (p->filename[i]);
      csp->functname[i] = strdup (p->functname[i]);
      csp->lineno[i] = p->lineno[i];
      csp->pc[i] = p->pc[i];
    }
    /* Insert new csp */
    h_insert (callsite_pc_cache, csp);
    return 1;
  } else {
    /* found the key */
    return 0;
  }
}

int
codecti_ht_insert_cs_src_id_cache (struct callsite_stats *p) {
  callsite_src_id_cache_entry_t key;
  callsite_src_id_cache_entry_t *csp;
  int i;

  for (i = 0; i < codecti.fullStackDepth; i++) {
    key.filename[i] = p->filename[i];
    key.functname[i] = p->functname[i];
    key.lineno[i] = p->lineno[i];
    key.pc[i] = p->pc[i];
  }

  if (h_search (callsite_pc_cache, &key, (void **) &csp) == NULL) {
    /* key not found */
    /* Allocate a new csp */
    csp =
        (callsite_src_id_cache_entry_t *)
        malloc (sizeof (callsite_src_id_cache_entry_t));
    bzero (csp, sizeof (callsite_src_id_cache_entry_t));

    /* Fill the csp */
    for (i = 0; i < codecti.fullStackDepth; i++) {
      csp->filename[i] = strdup (p->filename[i]);
      csp->functname[i] = strdup (p->functname[i]);
      csp->lineno[i] = p->lineno[i];
      csp->pc[i] = p->pc[i];
    }

    /* Insert new csp */
    h_insert (callsite_pc_cache, csp);
    return 1;
  } else {
    /* found the key */
    return 0;
  }
}

void
codecti_free_pc_cache() {
  if (NULL != callsite_pc_cache) {
      int ac, ndx;
      callsite_pc_cache_entry_t **av;

      h_drain(callsite_pc_cache, &ac, (void ***)&av);
      for (ndx = 0; ndx < ac; ndx++)
        {
          free(av[ndx]);
        }
      free(av);
    }
  return;
}

void
codecti_free_src_id_cache () {
  if (NULL != callsite_src_id_cache) {
      int ac, ndx;
      callsite_src_id_cache_entry_t **av;

      h_drain(callsite_src_id_cache, &ac, (void ***)&av);
      for (ndx = 0; ndx < ac; ndx++)
        {
          free(av[ndx]);
        }
      free(av);
    }
  return;
}

void codecti_cs_cache_fini()
{
  if (NULL != callsite_pc_cache) {
    h_close(callsite_pc_cache);
  }

  if (NULL != callsite_src_id_cache) {
    h_close(callsite_src_id_cache);
  }
}

static int
codecti_query_pc (void *pc, char **filename, char **functname, int *lineno)
{
  int rc = 0;
  char addr_buf[24];

#if defined(ENABLE_BFD) || defined(USE_LIBDWARF)
  if (codecti_find_src_loc (pc, filename, lineno, functname) == 0)
    {
      if (*filename == NULL || strcmp (*filename, "??") == 0)
        *filename = "[unknown]";

      if (*functname == NULL)
        *functname = "[unknown]";

      codecti_msg_debug
          ("Successful Source lookup for [%s]: %s, %d, %s\n",
            codecti_format_address (pc, addr_buf), *filename, *lineno,
            *functname);

    }
  else
    {
      codecti_msg_debug ("Unsuccessful Source lookup for [%s]\n",
                        codecti_format_address (pc, addr_buf));
      *filename = strdup ("[unknown]");
      *functname = strdup ("[unknown]");
      *lineno = 0;
    }
#else /* ! ENABLE_BFD || USE_LIBDWARF */
  *filename = strdup ("[unknown]");
  *functname = strdup ("[unknown]");
  *lineno = 0;
#endif

  if (*lineno == 0)
    rc = 1;			/* use this value to indicate a failed lookup */

  return rc;
}

/* take a callstats record (the pc) and determine src file, line.
 */
int
codecti_query_src (callsite_stats_t * p)
{
  int i;
  assert (p);

  for (i = 0; (i < codecti.fullStackDepth) && (p->pc[i] != NULL); i++)
    {
        codecti_query_pc (p->pc[i], &(p->filename[i]), &(p->functname[i]),
                        &(p->lineno[i]));
    }

  return 0;
}

/*

  <license>

  Copyright (c) 2006, The Regents of the University of California.
  Produced at the Lawrence Livermore National Laboratory
  Written by Jeffery Vetter and Christopher Chambreau.
  UCRL-CODE-223450.
  All rights reserved.

  Copyright (c) 2019, Mellanox Technologies Inc.
  Written by Artem Polyakov
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

/* EOF */
