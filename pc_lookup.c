/* -*- C -*- 
   Please see COPYRIGHT AND LICENSE information at the end of this file.

   ----- 

   pc_lookup.c -- functions that interface to bfd for source code lookup
   $Id$
 */

#ifndef lint
static char *svnid = "$Id$";
#endif

#if defined(DEMANGLE_GNU)
#include <ansidecl.h>
#endif

#include <assert.h>
#ifdef SO_LOOKUP
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codecti.h"
#include "codect-config.h"

#ifdef ENABLE_BFD
#ifndef CEXTRACT
#include "bfd.h"
#ifdef SO_LOOKUP
#include <search.h>
#endif
#endif
static asymbol **syms;
static bfd_vma pc;
static const char *filename;
static const char *functionname;
static unsigned int line;
static bfd *abfd = NULL;
static bfd *open_bfd_object (char *filename);

/*  BFD boolean and bfd_boolean types have changed through versions.
    It looks like bfd_boolean will be preferred.                     */
#ifdef HAVE_BFD_BOOLEAN
typedef boolean bfd_boolean;
#endif
#ifndef FALSE
#define FALSE 0
#endif
static bfd_boolean found;


#ifdef DEMANGLE_IBM

#include <demangle.h>
#include <string.h>

char *
codecti_demangle (const char *mangledSym)
{
  Name *ret;
  char *rest;
  unsigned long opts = 0x1;

  ret = demangle ((char *) mangledSym, &rest, opts);

  if (ret != NULL)
    return strdup (text (ret));
  else
    return NULL;
}

#elif DEMANGLE_Compaq

#include <demangle.h>
#include <string.h>

char *
codecti_demangle (const char *mangledSym)
{
  char out[1024];

  MLD_demangle_string (mangledSym, out, 1024,
		       MLD_SHOW_INFO | MLD_SHOW_DEMANGLED_NAME);

  return strdup (out);
}


#elif DEMANGLE_GNU

#ifdef HAVE_DEMANGLE_H
#include "demangle.h"
#else
#define DMGL_PARAMS    (1 << 0)
#define DMGL_ANSI      (1 << 1)
extern char *cplus_demangle (const char *mangled, int options);
#endif

char *
codecti_demangle (const char *mangledSym)
{
  return cplus_demangle (mangledSym, DMGL_ANSI | DMGL_PARAMS);
}

#endif


static void
find_address_in_section (abfd, section, data)
     bfd *abfd;
     asection *section;
     PTR data;
{
  bfd_vma vma;
  bfd_size_type size;
  bfd_vma local_pc = pc;
  char addr_buf1[24], addr_buf2[24], addr_buf3[24];

  assert (abfd);
  if (found)
    return;

#ifdef OSF1
  local_pc = pc | 0x100000000;
#elif defined(UNICOS_mp)
  /* use only least significant 32-bits */
  local_pc = pc & 0xFFFFFFFF;
#elif defined(AIX)
  local_pc = pc;
  if (codecti.obj_mode == 32)
    local_pc -= 0x10000000;
  else
    local_pc &= 0x00000000FFFFFFFF;
  local_pc += codecti.text_start;
  codecti_msg_debug ("pc is %s, text_start is %s, local_pc is %s\n",
                   codecti_format_address ((void *) pc, addr_buf1),
                   codecti_format_address ((void *) codecti.text_start, addr_buf2),
                   codecti_format_address ((void *) local_pc, addr_buf3));
#else
  local_pc = pc /*& (~0x10000000) */ ;
#endif

#if defined(HAVE_BFD_GET_SECTION_MACROS)
  if ((bfd_get_section_flags (abfd, section) & SEC_ALLOC) == 0)
  {
    codecti_msg_debug ("failed bfd_get_section_flags\n");
    return;
  }
#else
  if ((bfd_section_flags (section) & SEC_ALLOC) == 0)
  {
    codecti_msg_debug ("failed bfd_section_flags\n");
    return;
  }
#endif

#if defined(HAVE_BFD_GET_SECTION_MACROS)
  vma = bfd_get_section_vma (abfd, section);
#else
  vma = bfd_section_vma (section);
#endif

  if (local_pc < vma)
  {
    if (codect_debug == 1)
      {
        sprintf_vma (addr_buf1, local_pc);
        sprintf_vma (addr_buf2, vma);
#if defined(HAVE_BFD_GET_SECTION_MACROS)
        codecti_msg_debug
            ("failed bfd_get_section_vma: local_pc=%s  vma=%s\n",
             addr_buf1, addr_buf2);
#else
        codecti_msg_debug
           ("failed bfd_section_vma: local_pc=%s  vma=%s\n",
            addr_buf1, addr_buf2);
#endif
      }
    return;
  }

/* The following define addresses binutils modifications between
 * version 2.15 and 2.15.96
*/
#if defined(HAVE_BFD_GET_SECTION_SIZE)
  size = bfd_get_section_size (section);
#elif defined(HAVE_BFD_GET_SECTION_MACROS)
  if (section->reloc_done)
    size = bfd_get_section_size_after_reloc (section);
  else
    size = bfd_get_section_size_before_reloc (section);
#else
  size = bfd_section_size(section);
#endif

  if (local_pc >= vma + size)
  {
    if (codect_debug == 1)
      {
        sprintf_vma (addr_buf1, local_pc);
        sprintf_vma (addr_buf2, vma);
        sprintf_vma (addr_buf3, (vma + size));
        codecti_msg_debug ("PC not in section: pc=%s vma=%s-%s\n",
                         addr_buf1, addr_buf2, addr_buf3);
      }
    return;
  }


  found = bfd_find_nearest_line (abfd, section, syms, local_pc - vma,
                                 &filename, &functionname, &line);

  if (!found && codect_debug == 1)
  {
    sprintf_vma (addr_buf1, local_pc);
    sprintf_vma (addr_buf2, vma);
    sprintf_vma (addr_buf3, (vma + size));
    codecti_msg_debug ("bfd_find_nearest_line failed for : pc=%s vma=%s-%s\n",
                     addr_buf1, addr_buf2, addr_buf3);
  }

  if (codect_debug == 1)
  {
    sprintf_vma (addr_buf1, local_pc);
    sprintf_vma (addr_buf2, vma);
    sprintf_vma (addr_buf3, (vma + size));
    codecti_msg_debug ("bfd_find_nearest_line for : pc=%s vma=%s-%s\n",
                     addr_buf1, addr_buf2, addr_buf3);

    codecti_msg_debug ("                 returned : %s:%s:%u\n",
                     filename, functionname, line);
  }
}


#ifdef SO_LOOKUP

/*******************************************************************************
  The following functions support source code lookup for shared objects
   with SO info stored in a tree. 
*******************************************************************************/

/*  Used with twalk to print SO tree entries  */
static void
codecti_print_so_node_info (const void *so_node, VISIT which, int depth)
{
  so_info_t *cso;

  if ((so_info_t **) so_node == NULL)
    return;
  else
    cso = *(so_info_t **) so_node;

  switch (which)
    {
    case preorder:
      break;
    case postorder:
      printf ("%p - %p %lx: %s\n", cso->lvma, cso->uvma, cso->offset, cso->fpath);
      break;
    case endorder:
      break;
    case leaf:
      printf ("%p - %p %lx: %s\n", cso->lvma, cso->uvma, cso->offset, cso->fpath);
      break;
    }
}


/*  Print SO tree entries  */
static void
codecti_print_sos ()
{
  if (codecti.so_info == NULL)
    codecti_msg_warn ("Cannot print SOs as codecti.so_info is NULL\n");
  else
    twalk (codecti.so_info, codecti_print_so_node_info);
}


/*  For inserting into and searching SO tree  */
static int
codecti_so_info_compare (const void *n1, const void *n2)
{
  so_info_t *sn1, *sn2;
  int retval;

  sn1 = (so_info_t *) n1;
  sn2 = (so_info_t *) n2;


  if (sn1->lvma < sn2->lvma)
    retval = -1;
  else if (sn1->lvma > sn2->uvma)
    retval = 1;
  else
    retval = 0;

  codecti_msg_debug
    ("info_compare returning %d after comparing sn1->lvma %p to (sn2->lvma - sn2->uvma)  %p - %p\n",
     retval, sn1->lvma, sn2->lvma, sn2->uvma);

  return retval;
}


/*  Load map info for SOs  */
static int
codecti_parse_maps ()
{
  char fbuf[64];
  FILE *fh;
  void *lvma, *uvma;
  size_t offset;
  char *fpath, *inbuf = NULL, *tokptr;
  so_info_t *cso = NULL;
  size_t inbufsize;
  char *delim = " \n";
  char *scan_str;
  char *sp;

  snprintf (fbuf, 64, "/proc/%d/maps", (int) getpid ());

  fh = fopen (fbuf, "r");
  if (fh == NULL)
    {
      codecti_msg_warn ("Failed to get process map info from %s\n", fbuf);
      return 0;
    }

  if (sizeof (void *) == 4)
    scan_str = "%x-%x %*s %x";
  else
    scan_str = "%llx-%llx %*s %llx";

  codecti.so_info = NULL;

  while (getline (&inbuf, &inbufsize, fh) != -1)
    {
      /* sanity check input buffer */
      if (inbuf == NULL)
        return 0;

      codecti_msg_debug ("maps getline is %s\n", inbuf);

      /* scan address range */
      if (sscanf (inbuf, scan_str, &lvma, &uvma, &offset) < 2)
        return 0;

      codecti_msg_debug ("Parsed range as %lx - %lx offset: %lx\n", lvma, uvma, offset);

      /* get pointer to address range */
      tokptr = strtok_r (inbuf, delim, &sp);

      /* get pointer to permissions */
      tokptr = strtok_r (NULL, delim, &sp);

      /* Check for text segment */
      if (tokptr == NULL || tokptr[0] != 'r' || tokptr[2] != 'x')
        continue;

      /* get pointer to offset */
      tokptr = strtok_r (NULL, delim, &sp);

      /* get pointer to device */
      tokptr = strtok_r (NULL, delim, &sp);

      /* get pointer to inode */
      tokptr = strtok_r (NULL, delim, &sp);

      /* get pointer to file */
      fpath = strtok_r (NULL, delim, &sp);

      /* Process file info */
      if (fpath == NULL || fpath[0] != '/')
        continue;
      codecti_msg_debug ("maps fpath is %s\n", fpath);

      /* copy info into structure */
      cso = (so_info_t *) malloc (sizeof (so_info_t));
      if (cso == NULL)
        return 0;
      cso->lvma = lvma;
      cso->uvma = uvma;
      cso->offset = offset;
      cso->fpath = strdup (fpath);
      cso->bfd = NULL;
      if (tsearch (cso, (void **) &(codecti.so_info), codecti_so_info_compare) !=
          NULL)
        codecti.so_count++;
    }

  fclose (fh);

  if (inbuf != NULL)
    free (inbuf);

  if (codect_debug)
    codecti_print_sos ();

  return 1;
}
#endif /* ifdef SO_LOOKUP  */

int
codecti_find_src_loc (void *i_addr_hex, char **o_file_str, int *o_lineno,
                   char **o_funct_str)
{
  char buf[128];
  char addr_buf[24];

  if (i_addr_hex == NULL)
    {
      codecti_msg_debug
          ("codecti_find_src_loc returning failure as i_addr_hex == NULL\n");
      return 1;
    }

  /* Do initial source lookup check on executable file.
   * TODO: optimize lookup: should SO objects be checked first?
   */
  if (abfd == NULL)
    {
      codecti_msg_debug
          ("codecti_find_src_loc returning failure as abfd == NULL\n");
      return 1;
    }

  sprintf (buf, "%s", codecti_format_address (i_addr_hex, addr_buf));
  pc = bfd_scan_vma (buf, NULL, 16);

  found = FALSE;

  bfd_map_over_sections (abfd, find_address_in_section, (PTR) NULL);

#ifdef SO_LOOKUP
  if (!found)
    {
      so_info_t cso, *fso, **pfso;

      if (codecti.so_info == NULL)
        if (codecti_parse_maps () == 0)
          {
            codecti_msg_debug ("Failed to parse SO maps.\n");
            return 1;
          }

      cso.lvma = (void *) i_addr_hex;

      codecti_msg_debug
          ("At SO tfind, &cso is %p, &so_info is %p, &codecti_so_info_compare is %p\n",
           &cso, &(codecti.so_info), codecti_so_info_compare);
      pfso =
          (so_info_t **) tfind ((void *) &cso, (void **) &(codecti.so_info),
                                codecti_so_info_compare);
      codecti_msg_debug ("After SO tfind\n");

      if (pfso != NULL)
        {
          fso = *pfso;
          if (fso->bfd == NULL)
            {
              codecti_msg_debug ("opening SO filename %s\n", fso->fpath);
              fso->bfd = (bfd *) open_bfd_object (fso->fpath);
            }

          pc = (((char *) i_addr_hex - (char *) fso->lvma) + fso->offset);
          codecti_msg_debug
              ("Calling bfd_map_over_sections with new bfd for %p\n", pc);

          found = FALSE;

          codecti_msg_debug ("fso->bfd->sections is %p\n",
                           ((bfd *) (fso->bfd))->sections);
          bfd_map_over_sections (fso->bfd, find_address_in_section,
                                 (PTR) NULL);
        }

    }
#endif /* #ifdef SO_LOOKUP */

  if (found == 0)
    return 1;			/*  Failed to find source info.  */

  /* determine the function name */
  if (functionname == NULL || *functionname == '\0')
    {
      *o_funct_str = strdup ("[unknown]");
    }
  else
    {
      char *res = NULL;

#if defined(DEMANGLE_IBM) || defined(DEMANGLE_Compaq) || defined(DEMANGLE_GNU)
	  res = codecti_demangle (functionname);
#endif
	  if (res == NULL)
		{
		  *o_funct_str = strdup (functionname);
		}
	  else
		{
		  *o_funct_str = res;
		}

#if defined(DEMANGLE_IBM) || defined(DEMANGLE_Compaq) || defined(DEMANGLE_GNU)
	  codecti_msg_debug ("attempted demangle %s->%s\n", functionname,
	                   *o_funct_str);
#endif
	}

  /* set the filename and line no */
  if (codecti.baseNames == 0 && filename != NULL)
    {
      char *h;
      h = strrchr (filename, '/');
      if (h != NULL)
        filename = h + 1;
    }

  *o_lineno = line;
  *o_file_str = strdup (filename ? filename : "[unknown]");

  codecti_msg_debug ("BFD: %s -> %s:%u:%s\n", buf, *o_file_str, *o_lineno,
                   *o_funct_str);

  return 0;
}


static bfd *
open_bfd_object (char *filename)
{
  char *target = NULL;
  char **matching = NULL;
  long storage;
  long symcount;
  unsigned int size;
  static int bfd_initialized = 0;
  bfd *new_bfd;

  if (filename == NULL)
    {
      codecti_msg_warn ("BFD Object filename is NULL!\n");
      codecti_msg_warn
          ("If this is a Fortran application, you may be using the incorrect CodeCT library.\n");
      return NULL;
    }

#ifdef AIX

  /*  Kludge to get XCOFF text_start value to feed an approriate address
     value to bfd for looking up source info
   */
  codecti.text_start = codecti_get_text_start (filename);
#endif

  if (!bfd_initialized)
    {
      bfd_init ();
      bfd_initialized = 1;
    }

  /* set_default_bfd_target (); */

  codecti_msg_debug ("opening filename %s\n", filename);
  new_bfd = bfd_openr (filename, target);
  if (new_bfd == NULL)
    {
      codecti_msg_warn ("BFD could not open filename %s", filename);
      return NULL;
    }
  if (bfd_check_format (new_bfd, bfd_archive))
    {
      codecti_msg_warn ("can not get addresses from archive");
      bfd_close (new_bfd);
      return NULL;
    }
  if (!bfd_check_format_matches (new_bfd, bfd_object, &matching))
    {
      char *curr_match;
      if (matching != NULL)
        {
          for (curr_match = matching[0]; curr_match != NULL; curr_match++)
            codecti_msg_debug ("found matching type %s\n", curr_match);
          free (matching);
        }
      codecti_msg_warn ("BFD format matching failed");
      bfd_close (new_bfd);
      return NULL;
    }

  if ((bfd_get_file_flags (new_bfd) & HAS_SYMS) == 0)
    {
      codecti_msg_warn ("No symbols in the executable\n");
      bfd_close (new_bfd);
      return NULL;
    }

  /* TODO: move this to the begining of the process so that the user
     knows before the application begins */
  storage = bfd_get_symtab_upper_bound (new_bfd);
  if (storage < 0)
    {
      codecti_msg_warn ("storage < 0");
      bfd_close (new_bfd);
      return NULL;
    }

  symcount = bfd_read_minisymbols (new_bfd, FALSE, (void *) &syms, &size);
  if (symcount == 0)
    symcount =
        bfd_read_minisymbols (new_bfd, TRUE /* dynamic */ , (void *) &syms,
                              &size);

  if (symcount < 0)
    {
      codecti_msg_warn ("symcount < 0");
      bfd_close (new_bfd);
      return NULL;
    }
  else
    {
      codecti_msg_debug ("\n");
      codecti_msg_debug ("found %d symbols in file [%s]\n", symcount, filename);
    }

  return new_bfd;
}

int
open_bfd_executable (char *filename)
{
  abfd = open_bfd_object (filename);
  if (abfd == NULL)
    return 0;
  else
    return 1;
}

void
close_bfd_object (bfd * close_bfd)
{
  assert (close_bfd);
  bfd_close (close_bfd);
}

void
close_bfd_executable ()
{
  assert (abfd);
  bfd_close (abfd);
}

#elif !defined(USE_LIBDWARF)

int
codecti_find_src_loc (void *i_addr_hex, char **o_file_str, int *o_lineno,
		   char **o_funct_str)
{
  return 1;			/*  failure  */
}

#endif /* ENABLE_BFD */

#ifdef AIX
/*  Get the text_start value from the auxiliary header
    to provide BFD with pc values that match source information.
*/
#define __XCOFF32__
#define __XCOFF64__
#include <fcntl.h>
#include <filehdr.h>
#include <aouthdr.h>
#include <scnhdr.h>

static unsigned long long
codecti_get_text_start (char *filename)
{
  int fh;
  short magic;
  FILHDR FileHeader32;
  FILHDR_64 FileHeader64;
  AOUTHDR AoutHeader32;
  AOUTHDR_64 AoutHeader64;
  SCNHDR SectHeader32;
  SCNHDR_64 SectHeader64;

  unsigned long long text_start = 0;
  int count = 0;

  fh = open (filename, O_RDONLY);
  if (fh == -1)
    return 0;

  read (fh, &magic, 2);
  codecti_msg_debug ("magic is 0x%x\n", magic);
  lseek (fh, 0, 0);

  if (magic == 0x01DF)		/* 32-bit  */
    {
      codecti.obj_mode = 32;
      read (fh, &FileHeader32, sizeof (FILHDR));
      codecti_msg_debug ("aout size is %d\n", FileHeader32.f_opthdr);
      read (fh, &AoutHeader32, FileHeader32.f_opthdr);
      codecti_msg_debug ("text start is 0x%0x\n", AoutHeader32.o_text_start);

      while (count++ < FileHeader32.f_nscns)
        {
          read (fh, &SectHeader32, sizeof (SCNHDR));
          codecti_msg_debug ("found header name %s\n", SectHeader32.s_name);
          codecti_msg_debug ("found header raw ptr 0x%0x\n",
                           SectHeader32.s_scnptr);

          if (SectHeader32.s_flags & STYP_TEXT)
            text_start = AoutHeader32.o_text_start - SectHeader32.s_scnptr;
        }
    }
  else if (magic == 0x01EF || magic == 0x01F7)	/*  64-bit  */
    {
      codecti.obj_mode = 64;
      read (fh, &FileHeader64, sizeof (FILHDR_64));
      codecti_msg_debug ("aout size is %d\n", FileHeader64.f_opthdr);
      read (fh, &AoutHeader64, FileHeader64.f_opthdr);
      codecti_msg_debug ("text start is 0x%0llx\n", AoutHeader64.o_text_start);

      while (count++ < FileHeader64.f_nscns)
        {
          read (fh, &SectHeader64, sizeof (SCNHDR_64));
          codecti_msg_debug ("found header name %s\n", SectHeader64.s_name);
          codecti_msg_debug ("found header raw ptr 0x%0llx\n",
                           SectHeader64.s_scnptr);

          if (SectHeader64.s_flags & STYP_TEXT)
            text_start = AoutHeader64.o_text_start - SectHeader64.s_scnptr;
        }
    }
  else
    {
      codecti_msg_debug ("invalid magic number.\n");
      return 0;
    }

  codecti_msg_debug ("text_start is 0x%0llx\n", text_start);
  close (fh);

  return text_start;
}

#endif



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
